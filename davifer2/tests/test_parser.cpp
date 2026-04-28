#include "RequestParser.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "MimeTypes.hpp"
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstring>

static int g_tests_passed = 0;
static int g_tests_failed = 0;

static void test_ok(const char* name)
{
	g_tests_passed++;
	std::cout << "  [OK] " << name << std::endl;
}

static void test_fail(const char* name, const char* detail)
{
	g_tests_failed++;
	std::cerr << "  [FAIL] " << name << ": " << detail << std::endl;
}

#define ASSERT_EQ(a, b, name) \
	if ((a) == (b)) { test_ok(name); } \
	else { test_fail(name, "values not equal"); }

#define ASSERT_TRUE(cond, name) \
	if ((cond)) { test_ok(name); } \
	else { test_fail(name, "condition false"); }

// ─── HTTPStatus Tests ────────────────────────────────────────────────────────

static void test_status_codes()
{
	std::cout << "\n=== HTTPStatus ===" << std::endl;
	ASSERT_EQ(HTTPStatus::getReasonPhrase(200), "OK", "200 OK");
	ASSERT_EQ(HTTPStatus::getReasonPhrase(404), "Not Found", "404 Not Found");
	ASSERT_EQ(HTTPStatus::getReasonPhrase(500), "Internal Server Error", "500 ISE");
	ASSERT_EQ(HTTPStatus::getReasonPhrase(301), "Moved Permanently", "301 redirect");
	ASSERT_EQ(HTTPStatus::getReasonPhrase(413), "Content Too Large", "413 body too large");
	ASSERT_EQ(HTTPStatus::getReasonPhrase(9999), "Unknown", "unknown code");
	ASSERT_TRUE(HTTPStatus::isValidCode(200), "200 is valid");
	ASSERT_TRUE(!HTTPStatus::isValidCode(999), "999 is not valid");
}

// ─── MimeTypes Tests ─────────────────────────────────────────────────────────

static void test_mime_types()
{
	std::cout << "\n=== MimeTypes ===" << std::endl;
	ASSERT_EQ(MimeTypes::getMimeType(".html"), "text/html", "html mime");
	ASSERT_EQ(MimeTypes::getMimeType(".css"), "text/css", "css mime");
	ASSERT_EQ(MimeTypes::getMimeType(".js"), "application/javascript", "js mime");
	ASSERT_EQ(MimeTypes::getMimeType(".png"), "image/png", "png mime");
	ASSERT_EQ(MimeTypes::getMimeType(".xyz"), "application/octet-stream", "unknown ext");
	ASSERT_EQ(MimeTypes::getExtensionFromPath("/style.css"), ".css", "ext from path");
	ASSERT_EQ(MimeTypes::getExtensionFromPath("/no-ext"), "", "no ext");
	ASSERT_EQ(MimeTypes::getExtensionFromPath("/dir.d/file"), "", "dot in dir");
}

// ─── Simple GET Request ──────────────────────────────────────────────────────

static void test_simple_get()
{
	std::cout << "\n=== Simple GET ===" << std::endl;
	RequestParser parser;
	parser.feed("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");

	ASSERT_TRUE(parser.isComplete(), "parsing complete");
	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getMethod(), "GET", "method is GET");
	ASSERT_EQ(req.getPath(), "/index.html", "path");
	ASSERT_EQ(req.getHttpVersion(), "HTTP/1.1", "version");
	ASSERT_EQ(req.getHeader("host"), "localhost", "host header");
	ASSERT_EQ(req.getErrorCode(), 0, "no error");
	ASSERT_TRUE(req.getBody().empty(), "no body");
}

// ─── GET with Query String ───────────────────────────────────────────────────

static void test_get_query()
{
	std::cout << "\n=== GET with Query ===" << std::endl;
	RequestParser parser;
	parser.feed("GET /search?q=hello&page=2 HTTP/1.1\r\nHost: localhost\r\n\r\n");

	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getPath(), "/search", "path without query");
	ASSERT_EQ(req.getQueryString(), "q=hello&page=2", "query string");
}

// ─── POST with Body ──────────────────────────────────────────────────────────

static void test_post_body()
{
	std::cout << "\n=== POST with Body ===" << std::endl;
	RequestParser parser;
	std::string body = "name=david&project=webserv";
	std::ostringstream oss;
	oss << "POST /submit HTTP/1.1\r\n"
		<< "Host: localhost\r\n"
		<< "Content-Length: " << body.size() << "\r\n"
		<< "Content-Type: application/x-www-form-urlencoded\r\n"
		<< "\r\n"
		<< body;

	parser.feed(oss.str());

	ASSERT_TRUE(parser.isComplete(), "parsing complete");
	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getMethod(), "POST", "method is POST");
	ASSERT_EQ(req.getBody(), body, "body matches");
	ASSERT_EQ(req.getErrorCode(), 0, "no error");
}

// ─── Incremental Feed ────────────────────────────────────────────────────────

static void test_incremental()
{
	std::cout << "\n=== Incremental Feed ===" << std::endl;
	RequestParser parser;

	parser.feed("GET /pa");
	ASSERT_TRUE(!parser.isComplete(), "not complete after partial line");

	parser.feed("th HTTP/1.1\r\n");
	ASSERT_TRUE(!parser.isComplete(), "not complete after request line");

	parser.feed("Host: loc");
	ASSERT_TRUE(!parser.isComplete(), "not complete mid-header");

	parser.feed("alhost\r\n\r\n");
	ASSERT_TRUE(parser.isComplete(), "complete after final CRLF");

	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getPath(), "/path", "path correct");
	ASSERT_EQ(req.getHeader("host"), "localhost", "host correct");
}

// ─── Chunked Transfer ────────────────────────────────────────────────────────

static void test_chunked()
{
	std::cout << "\n=== Chunked Transfer ===" << std::endl;
	RequestParser parser;

	std::string request =
		"POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Transfer-Encoding: chunked\r\n"
		"\r\n"
		"5\r\n"
		"Hello\r\n"
		"6\r\n"
		" World\r\n"
		"0\r\n"
		"\r\n";

	parser.feed(request);
	ASSERT_TRUE(parser.isComplete(), "chunked complete");
	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getBody(), "Hello World", "chunked body decoded");
	ASSERT_EQ(req.getErrorCode(), 0, "no error");
}

// ─── Chunked Incremental ────────────────────────────────────────────────────

static void test_chunked_incremental()
{
	std::cout << "\n=== Chunked Incremental ===" << std::endl;
	RequestParser parser;

	parser.feed("POST /upload HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n");
	ASSERT_TRUE(!parser.isComplete(), "not complete before chunks");

	parser.feed("5\r\nHel");
	ASSERT_TRUE(!parser.isComplete(), "not complete mid-chunk");

	parser.feed("lo\r\n0\r\n\r\n");
	ASSERT_TRUE(parser.isComplete(), "complete after terminal chunk");

	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getBody(), "Hello", "chunked incremental body");
}

// ─── Error Cases ─────────────────────────────────────────────────────────────

static void test_errors()
{
	std::cout << "\n=== Error Cases ===" << std::endl;

	// Missing host header
	{
		RequestParser p;
		p.feed("GET / HTTP/1.1\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 400, "missing host = 400");
	}

	// Unsupported method
	{
		RequestParser p;
		p.feed("PATCH /foo HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 405, "unsupported method = 405");
	}

	// Bad HTTP version
	{
		RequestParser p;
		p.feed("GET / HTTP/2.0\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 505, "bad version = 505");
	}

	// POST without Content-Length
	{
		RequestParser p;
		p.feed("POST /submit HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 411, "POST no CL = 411");
	}

	// Body too large
	{
		RequestParser p;
		p.setMaxBodySize(10);
		p.feed("POST /submit HTTP/1.1\r\nHost: x\r\nContent-Length: 100\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 413, "body too large = 413");
	}

	// URI without leading /
	{
		RequestParser p;
		p.feed("GET foo HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getErrorCode(), 400, "no leading slash = 400");
	}
}

// ─── Percent Decode ──────────────────────────────────────────────────────────

static void test_percent_decode()
{
	std::cout << "\n=== Percent Decode ===" << std::endl;
	RequestParser parser;
	parser.feed("GET /hello%20world HTTP/1.1\r\nHost: x\r\n\r\n");
	ASSERT_EQ(parser.getRequest().getPath(), "/hello world", "space decoded");
}

// ─── Path Normalization ──────────────────────────────────────────────────────

static void test_path_normalize()
{
	std::cout << "\n=== Path Normalization ===" << std::endl;

	{
		RequestParser p;
		p.feed("GET /foo/../bar HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getPath(), "/bar", ".. resolved");
	}
	{
		RequestParser p;
		p.feed("GET /foo/./bar HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getPath(), "/foo/bar", ". resolved");
	}
	{
		RequestParser p;
		p.feed("GET /../../etc/passwd HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_EQ(p.getRequest().getPath(), "/etc/passwd", "traversal clamped");
	}
}

// ─── HTTPResponse Tests ──────────────────────────────────────────────────────

static void test_response()
{
	std::cout << "\n=== HTTPResponse ===" << std::endl;

	HTTPResponse resp(200);
	resp.setHeader("Content-Type", "text/html");
	resp.setBody("<h1>Hello</h1>");

	std::string serialized = resp.serialize();
	ASSERT_TRUE(serialized.find("HTTP/1.1 200 OK\r\n") == 0, "status line");
	ASSERT_TRUE(serialized.find("Content-Type: text/html\r\n") != std::string::npos, "content-type header");
	ASSERT_TRUE(serialized.find("Content-Length: 14\r\n") != std::string::npos, "content-length auto-set");
	ASSERT_TRUE(serialized.find("<h1>Hello</h1>") != std::string::npos, "body present");
}

static void test_error_response()
{
	std::cout << "\n=== Error Response ===" << std::endl;
	HTTPResponse resp = HTTPResponse::buildErrorResponse(404);
	std::string s = resp.serialize();
	ASSERT_TRUE(s.find("HTTP/1.1 404 Not Found") == 0, "404 status line");
	ASSERT_TRUE(s.find("Connection: close") != std::string::npos, "connection close");
	ASSERT_TRUE(s.find("<h1>404 Not Found</h1>") != std::string::npos, "error page content");
}

static void test_redirect_response()
{
	std::cout << "\n=== Redirect Response ===" << std::endl;
	HTTPResponse resp = HTTPResponse::buildRedirectResponse(301, "/new-location");
	std::string s = resp.serialize();
	ASSERT_TRUE(s.find("HTTP/1.1 301") == 0, "301 status line");
	ASSERT_TRUE(s.find("Location: /new-location") != std::string::npos, "location header");
}

// ─── Keep-Alive ──────────────────────────────────────────────────────────────

static void test_keepalive()
{
	std::cout << "\n=== Keep-Alive ===" << std::endl;

	// HTTP/1.1 default is keep-alive
	{
		RequestParser p;
		p.feed("GET / HTTP/1.1\r\nHost: x\r\n\r\n");
		ASSERT_TRUE(p.getRequest().shouldKeepAlive(), "1.1 default keepalive");
	}
	// HTTP/1.1 with Connection: close
	{
		RequestParser p;
		p.feed("GET / HTTP/1.1\r\nHost: x\r\nConnection: close\r\n\r\n");
		ASSERT_TRUE(!p.getRequest().shouldKeepAlive(), "1.1 conn close");
	}
	// HTTP/1.0 default is close
	{
		RequestParser p;
		p.feed("GET / HTTP/1.0\r\nHost: x\r\n\r\n");
		ASSERT_TRUE(!p.getRequest().shouldKeepAlive(), "1.0 default close");
	}
}

// ─── Parser Reset ────────────────────────────────────────────────────────────

static void test_reset()
{
	std::cout << "\n=== Parser Reset ===" << std::endl;
	RequestParser parser;

	parser.feed("GET /first HTTP/1.1\r\nHost: x\r\n\r\n");
	ASSERT_TRUE(parser.isComplete(), "first request complete");
	ASSERT_EQ(parser.getRequest().getPath(), "/first", "first path");

	parser.reset();
	parser.feed("GET /second HTTP/1.1\r\nHost: x\r\n\r\n");
	ASSERT_TRUE(parser.isComplete(), "second request complete");
	ASSERT_EQ(parser.getRequest().getPath(), "/second", "second path");
}

// ─── DELETE Method ───────────────────────────────────────────────────────────

static void test_delete()
{
	std::cout << "\n=== DELETE ===" << std::endl;
	RequestParser parser;
	parser.feed("DELETE /resource/42 HTTP/1.1\r\nHost: localhost\r\n\r\n");
	HTTPRequest req = parser.getRequest();
	ASSERT_EQ(req.getMethod(), "DELETE", "method is DELETE");
	ASSERT_EQ(req.getPath(), "/resource/42", "delete path");
	ASSERT_EQ(req.getErrorCode(), 0, "no error");
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main()
{
	std::cout << "=============================" << std::endl;
	std::cout << " Webserv HTTP Module Tests" << std::endl;
	std::cout << "=============================" << std::endl;

	test_status_codes();
	test_mime_types();
	test_simple_get();
	test_get_query();
	test_post_body();
	test_incremental();
	test_chunked();
	test_chunked_incremental();
	test_errors();
	test_percent_decode();
	test_path_normalize();
	test_response();
	test_error_response();
	test_redirect_response();
	test_keepalive();
	test_reset();
	test_delete();

	std::cout << "\n=============================" << std::endl;
	std::cout << " Results: " << g_tests_passed << " passed, "
			  << g_tests_failed << " failed" << std::endl;
	std::cout << "=============================" << std::endl;

	return g_tests_failed > 0 ? 1 : 0;
}
