/*

#include "web_server.hpp" // Inclou la capçalera principal amb les definicions de classe

// Constructor per defecte: inicialitza una petició buida i valors base
Response::Response() : _request(*(new Request())), _serverConfig(NULL)
{
    _full_path = "";        // Camí complet al fitxer (inicialment buit)
    _body.clear();          // Neteja el vector del cos de la resposta
    _redirectCode = 0;      // Codi de redirecció o estat (0 = no definit)
    _redirectUrl = "";      // URL de redirecció buida
    _cgiFlag = 0;           // Bandera per indicar si és un script CGI
    _contentLength = 0;     // Mida del contingut inicial a 0
    _autoindex = false;     // Llistat de directoris desactivat per defecte
}

// Constructor de còpia: utilitza l'operador d'assignació
Response::Response(const Response &src) : _request(src._request)
{
    *this = src; // Copia tots els atributs de l'objecte font
}

// Constructor parametritzat: rep la petició del client i la configuració del servidor
Response::Response(const Request &req, const ServerConfig *serConf) : _request(req), _serverConfig(serConf)
{
    _full_path = "";        // Inicialitza el camí buit
    _body.clear();          // Neteja el cos
    _redirectCode = 0;      // Inicialitza el codi d'estat
    _redirectUrl = "";      // Inicialitza la URL de redirecció
    _cgiFlag = 0;           // Inicialitza la bandera CGI
    _contentLength = 0;     // Inicialitza la mida del contingut
    _autoindex = false;     // Inicialitza l'autoindex
}

// Operador d'assignació: copia els valors d'una altra Response evitant l'auto-assignació
Response    &Response::operator=(const Response &src)
{
    if (this != &src) // Comprova que no s'estigui assignant a si mateix
    {
        _full_path = src._full_path;       // Copia el camí del fitxer
        _body = src._body;                 // Copia les dades del cos
        _redirectCode = src._redirectCode; // Copia el codi d'estat
        _redirectUrl = src._redirectUrl;   // Copia la URL de redirecció
        _cgiFlag = src._cgiFlag;           // Copia l'estat CGI
        _contentLength = src._contentLength; // Copia la mida
        _autoindex = src._autoindex;       // Copia l'estat de l'autoindex
        _serverConfig = src._serverConfig; // Copia el punter a la configuració
        _location = src._location;         // Copia la configuració de ruta específica
    }
    return (*this); // Retorna la referència a l'objecte actual
}

// Destructor virtual buit
Response::~Response(){}

/* ROUTER (PATH MATCHING I TARGET RESOLVE )

// Funció principal per determinar quin fitxer s'ha de servir segons la URL
int Response::buildPath()
{
    if (!_serverConfig) // Si no hi ha configuració carregada, error intern
    {
        std::cout << "Error: No hi ha configuracio carregada a la Response " << std::endl;
        _redirectCode = 500; // Defineix error 500
        return (1); // Retorna error
    }
    std::string _urlMatch = ""; // Variable per guardar la millor coincidència de ruta
    // 1. BUSQUEM LA URL: troba la 'location' que millor encaixa amb la ruta de la petició
    findURL(_request.getPath(), _serverConfig->getLocations(), _urlMatch);
    if (_urlMatch.empty()) // Si no hi ha cap coincidència, la pàgina no existeix
    {
        _redirectCode = 404; // Defineix error 404
        return (1); // Retorna error
    }
    // 2. OBTENIR LA LOCATION REAL: busca l'objecte de configuració específic per a la ruta trobada
    const LocationConfig *urlAcces = NULL; // Punter temporal a la configuració de la ruta
    const std::vector<LocationConfig> &locs = _serverConfig->getLocations(); // Llista de totes les rutes
    for (size_t i = 0; i < locs.size(); i++) // Recorre les rutes configurades
    {
        if (locs[i].getPath() == _urlMatch) // Si coincideix amb la millor ruta trobada
        {
            urlAcces = &locs[i]; // Guarda el punter
            _location = locs[i]; // Guarda una còpia a l'objecte Response
            break; // Surt del bucle
        }
    }
    if (!urlAcces) // Seguretat: si no s'ha trobat l'objecte, 404
    {
        _redirectCode = 404;
        return (1);
    }
    // Comprova si el mètode HTTP (GET, POST...) està permès en aquesta ruta
    if (validMethods(_request.getMethods(), urlAcces,_redirectCode)) 
    {
        std::cout << "NOT ALLOWED METHOD " << std::endl;
        return (1); // Retorna error (normalment 405)
    }
    // 3. VALIDAR EL MAX SIZE BODY: comprova que el client no enviï dades massa grans
    if (_request.getBodyString().length() > _serverConfig->getClientMaxBodySize()) 
    {
        _redirectCode = 413; // Request Entity Too Large
        return (1);
    }
    // 4. VALIDAR REDIRECCIONS: si la ruta diu que s'ha de redirigir a un altre lloc
    if (urlAcces->getRedirectCode() != 0) 
    {
        _redirectCode = urlAcces->getRedirectCode(); // Codi 301, 302...
        _redirectUrl = urlAcces->getRedirectUrl();   // Nova destinació
        return (1);
    }
    // 5. SI CONTE CGI -BIN: identifica rutes especials per a scripts executables
    if (urlAcces->getPath().find("cgi-bin") != std::string::npos) 
    {
        _cgiFlag = 1; // Marca com a CGI
        return (0); // Continua el procés sense carregar fitxer estàtic
    }
    // 6. CONSTRUIR PATH AL FITXE REAL: concatena el directori arrel amb la ruta demanada
    _full_path = urlAcces->getRoot() + _request.getPath();
    // 7. CGI PER EXTENSIO: identifica si el fitxer acaba en una extensió CGI (.php, .py...)
    if (!urlAcces->getCgiExtension().empty()) 
    {
        std::string ext = urlAcces->getCgiExtension(); // Obté l'extensió configurada
        if (_full_path.length() >= ext.length() && 
            _full_path.compare(_full_path.length() - ext.length(), ext.length(), ext) == 0) {
            _cgiFlag = 1; // Marca com a CGI
            return (0);
        }
    }
    // 8. DIRECTORIS I INDEX: si el camí és una carpeta, busca un fitxer índex
    if (isDirectory(_full_path)) 
    {
        // Si el camí de la carpeta no acaba en '/', redirigeix per normalitzar la URL
        if (_full_path.empty() || _full_path[_full_path.length() - 1] != '/') 
        {
            _redirectCode = 301; // Redirecció permanent
            _redirectUrl = _request.getPath() + "/"; // Afegeix la barra final
            return (1);
        }
        // Recorre la llista de fitxers índex (ex: index.html, index.htm)
        const std::vector<std::string> &indexFile = urlAcces->getIndexFiles();
        std::string foundIndex = "";
        for (size_t i = 0; i < indexFile.size(); i++)
        {
            std::string fullIndexPath = _full_path + indexFile[i]; // Camí complet a l'índex
            if (fileExist(fullIndexPath)) // Comprova si el fitxer existeix realment
            {
                foundIndex = indexFile[i]; // Guarda el nom trobat
                _full_path = fullIndexPath; // Actualitza el path al fitxer final
                break ; // Deixa de buscar
            }
        }
        // Si no s'ha trobat cap fitxer índex vàlid
        if (foundIndex.empty()) 
        {
            if (urlAcces->getAutoindex()) // Comprova si el llistat automàtic està activat
            {
                _autoindex = true; // Activa el generador d'HTML de directori
                return (0);
            } 
            else 
            {
                _redirectCode = 403; // Accés prohibit si no hi ha índex ni llistat
                return (1);
            }
        }
    }
    // 9. FITXER EXISTEIX: comprova finalment si el recurs demanat existeix al disc
    if (!fileExist(_full_path)) 
    {
        _redirectCode = 404; // Not Found
        return (1);
    }
    return (0); // Tot correcte, el camí està preparat
}

/* GESTIO DEL CONTIGUT (BUILD BODY I HANDLERS)

// Llegeix el contingut del fitxer apuntat per _full_path i el posa al body
int Response::readFile()
{
    // Obre el fitxer en mode binari i situa el punter al final per saber la mida
    std::ifstream file(_full_path.c_str(), std::ios::binary | std::ios::ate);
    if (!file) // Si no es pot obrir
    {
        _redirectCode = 404; // Marca error
        return (1);
    }   
    std::streamsize size = file.tellg(); // Obté la mida actual del fitxer
    file.seekg(0, std::ios::beg); // Torna al principi del fitxer
    _body.resize(size); // Reserva espai al vector _body segons la mida del fitxer
    if (!file.read(reinterpret_cast<char*>(&_body[0]), size)) // Llegeix les dades
    {
        _redirectCode = 500; // Error de lectura
        return (1);
    }
    _contentLength = _body.size(); // Defineix la longitud final del contingut
    return (0); // Lectura finalitzada amb èxit
}

// Construeix el cos de la resposta segons el mètode i el camí
int Response::buildBody()
{
    try 
    {
        // 1. EXECUTEM buildPath per determinar què fer i on anar
        if (buildPath())
        {
            buildErrorBody(); // Si falla, genera el cos d'error (pàgina 404, etc)
            return (1);
        }
        // 2. Si és CGI o Autoindex, sortim; el contingut es generarà dinàmicament després
        if (_cgiFlag || _autoindex)
            return (0);
        // 3. Si ja hi ha un codi especial definit (redireccions o errors previs), sortim
        if (_redirectCode != 0 && _redirectCode != 200)
            return (0);
        // 4. GESTIÓ SEGONS EL MÈTODE HTTP
        std::string method_str = methodToStr(_request.getMethods()); // Passa l'enum a string
        if (method_str == "GET") // Si el client demana dades
        {
            if (readFile()) // Intenta llegir el fitxer estàtic
            {
                _redirectCode = 404;
                buildErrorBody();
                return (1);
            }
        }
        else if (method_str == "POST" || method_str == "PUT") // Si el client envia dades
        {
            // Obre un fitxer per escriure les dades rebudes del client
            std::ofstream file(_full_path.c_str(), std::ios::binary);
            if (file.fail()) 
            {
                _redirectCode = 500;
                buildErrorBody();
                return (1);
            }
            // Gestió de dades tipus form-data/multipart
            if (_request.getMultiformFlag()) 
            {
                // Neteja els límits (boundaries) de la petició per treure només el fitxer pur
                std::vector<uint8_t> cleanData = removeBoundary(_request.getBodyBits(), _request.getBoundary());
                file.write(reinterpret_cast<const char*>(&cleanData[0]), cleanData.size());
            }
            else // Si són dades binàries pures
            {
                const std::vector<uint8_t>& rawBody = _request.getBodyBits();
                if (!rawBody.empty())
                    file.write(reinterpret_cast<const char*>(&rawBody[0]), rawBody.size());
            }
            file.close(); // Tanca el fitxer guardat
            if (fileExist(_full_path)) // Si el fitxer existeix després d'escriure
                _redirectCode = 200; // OK
            else
                _redirectCode = 201; // Created
        }
        _contentLength = _body.size(); // Actualitza la mida final
        if (_redirectCode == 0) // Si no hi ha codi, per defecte és 200
            _redirectCode = 200;
    }
    catch (const Response::BodyEmptyException &e) // Captura errors de cos buit
    {
        _redirectCode = 400; // Bad Request
        buildErrorBody();
        return (1);
    }
    return (0);
}

/* CAPÇALERA 

// Construeix l'string amb totes les capçaleres HTTP obligatòries
void Response::setHeaders() {
    // 3.1 Status Line: Ex "HTTP/1.1 200 OK"
    _contentResponse = "HTTP/1.1 " + codeToStr(_redirectCode) + " " + statusCodeToString(_redirectCode) + "\r\n";   
    // 3.2 Server Name: Informació del servidor
    _contentResponse += "Server: Webserv/1.0 (Unix)\r\n";
    // 3.3 Date: Data actual en format HTTP (GMT)
    char buf[100];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    _contentResponse += "Date: " + std::string(buf) + "\r\n";
    // 3.4 Content-Type: Identifica si és text/html, image/jpeg, etc segons l'extensió
    _contentType = _mime.getMimeType(_full_path);
    _contentResponse += "Content-Type: " + _contentType + "\r\n";
    // 3.5 Content-Length: Indica quants bytes té el body
    std::stringstream ss;
    ss << _contentLength;
    _contentResponse += "Content-Length: " + ss.str() + "\r\n";
    // 3.6 Connection: Defineix si tanquem o mantenim la connexió
    _contentResponse += "Connection: keep-alive\r\n\r\n"; // El doble \r\n separa headers de body
}

/* GESTIO DE ERRORS

// Busca i carrega la pàgina HTML d'error personalitzada definida al fitxer .conf
void    Response::buildErrorBody()
{
    // Si la configuració té una pàgina definida per a aquest codi d'error
    if (_serverConfig && _serverConfig->getErrorPages().count(_redirectCode))
    {
        // Construeix el camí a la pàgina d'error
        _full_path = _location.getRoot() + _serverConfig->getErrorPages().at(_redirectCode);
        if (readFile()) // Intenta llegir-la
            return ;
    }
}

/* CONSTRUCCIO RESPOSTA

// Genera dinàmicament una pàgina HTML amb la llista de fitxers d'un directori
int Response::buildHtmlIndex()
{
    struct dirent   *entityStruct; // Estructura per llegir entrades del directori
    DIR             *directory;    // Punter al directori obert
    std::string     dirListPage;   // String per anar muntant l'HTML
    
    directory = opendir(_full_path.c_str()); // Obre la carpeta
    if (directory == NULL) // Si falla l'obertura (ex: falta de permisos)
    {    
        return (1);
    }

    // Inici del document HTML
    dirListPage.append("<html>\n<head>\n<title>Index of " + _full_path + "</title>\n");
    dirListPage.append("<style>body{font-family:sans-serif;} table{width:80%; margin:20px auto;} th{text-align:left; border-bottom:1px solid #ccc;}</style>\n");
    dirListPage.append("</head>\n<body>\n");
    dirListPage.append("<h1>Index of " + _request.getPath() + "</h1>\n<hr>\n");
    dirListPage.append("<table>\n<tr><th>File Name</th><th>Last Modification</th><th>File Size</th></tr>\n");

    struct stat file_stat; // Estructura per detalls del fitxer (data, mida)
    std::string file_path;

    while((entityStruct = readdir(directory)) != NULL) // Llegeix cada fitxer/carpeta dins
    {
        if(std::string(entityStruct->d_name) == ".") // Salta l'entrada del directori actual (.)
            continue;

        file_path = _full_path; // Camí base
        if (file_path[file_path.length() - 1] != '/')
            file_path += "/"; // Afegeix barra si cal
        file_path += entityStruct->d_name; // Nom del fitxer actual

        if (stat(file_path.c_str() , &file_stat) == -1) // Obté la info del fitxer (mida, data)
            continue;

        dirListPage.append("<tr>\n<td>\n"); // Cel·la del nom amb enllaç
        dirListPage.append("<a href=\"");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode)) // Si és carpeta, afegeix barra visual
            dirListPage.append("/");
        dirListPage.append("\">");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode))
            dirListPage.append("/");
        dirListPage.append("</a>\n</td>\n<td>\n");

        dirListPage.append(ctime(&file_stat.st_mtime)); // Converteix timestamp a data llegible
        dirListPage.append("</td>\n<td>\n");

        if (!S_ISDIR(file_stat.st_mode)) // Si no és directori, mostra la mida
        {
            std::stringstream ss;
            ss << file_stat.st_size;
            dirListPage.append(ss.str() + " bytes");
        }
        else // Si és carpeta, no mostra mida
            dirListPage.append("-");

        dirListPage.append("</td>\n</tr>\n");
    }
    
    dirListPage.append("</table>\n<hr>\n</body>\n</html>\n"); // Tanca l'HTML
    closedir(directory); // Tanca l'accés al directori

    _body.clear(); // Neteja el body
    // Omple el body de la resposta amb l'HTML que acabem de generar
    _body.insert(_body.begin(), dirListPage.begin(), dirListPage.end());
    
    _contentLength = _body.size(); // Actualitza la mida del nou HTML
    _contentType = "text/html";    // Força el tipus a HTML

    return (0);
}

// Funció mestre que orquestra tota la creació de la resposta
void Response::buildResponse()
{
    if (buildBody()) // Tenta resoldre la ruta i el cos
        buildErrorBody(); // Si falla buildBody, assegura que hi hagi un cos d'error
    
    if (_cgiFlag) // Si és CGI, no fem res més aquí (ho gestionarà el mòdul CGI)
        return;

    if (_autoindex) // Si s'ha determinat que cal fer llistat de fitxers
    {
        if (buildHtmlIndex()) // Genera el llistat HTML
        {
            _redirectCode = 500; // Si falla generant el llistat, error intern
            buildErrorBody();
        }
        else
            _redirectCode = 200; // Llistat generat correctament
    }

    setHeaders(); // Un cop tenim el body i el codi final, generem les capçaleres

    // Ajunta les capçaleres amb el cos dins de l'string final de resposta (_contentResponse)
    // Només s'adjunta el cos si és un GET o si és una resposta d'error
    if ((_request.getMethods() == GET || _redirectCode != 200))
        _contentResponse.append(_body.begin(), _body.end());
}

*/