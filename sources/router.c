#include "enki.h"

extern struct mg_mgr	g_mgr;
static struct s_json	*g_user_json;
char					g_token[65];
char					g_status[4];
char					*g_json_string;
char 					*g_user;

/* Handlers */
void	handle_routes(struct mg_connection *c, struct mg_http_message *request)
{
	struct mg_http_serve_opts	opts = {.mime_types = "text=text/html", .extra_headers = "AA: bb\r\nCC: dd\r\n"};
	int							uri;
	int							method;

	uri = get_uri(request);
	method = get_method(request);

	if (method < 0)
		mg_http_reply(c, 400, "", "");
	else if (uri < 0)
		mg_http_reply(c, 404, "", "");
	else if (uri == HOME_ROUTE && method == GET)
		mg_http_reply(c, 308, "location:/docs\r\n", "");
	else if (uri == DOCS_ROUTE_ && method == GET)
		mg_http_reply(c, 308, "location:/docs\r\n", "");
	else if (uri == DOCS_ROUTE && method == GET)
		mg_http_serve_file(c, request, DOCS_FILE, &opts);
	else if (uri == API_BASE_ROUTE || uri == API_BASE_ROUTE_)
		mg_http_reply(c, 404, "", "");
	else if (uri == API_MAIN_ROUTE && method == GET)
		handle_main_route(c, request);
	else
		mg_http_reply(c, 405, "", "");
}

void	handle_main_route(struct mg_connection *c, struct mg_http_message *request)
{
	g_user = get_user(request);
	intra_token(&g_mgr, token_cb);
	intra_user(&g_mgr, user_cb);
	if (g_json_string)
	{
		handle_database_crud(g_json_string);
		mg_http_reply(c, atoi(g_status), "content-type: application/json; charset=utf-8\r\n", g_json_string);
		free(g_user);
		free(g_json_string);
		g_json_string = NULL;
	}
	mg_http_reply(c, atoi(g_status), "", "");
}

/* Utilities */
int		create_main_route_response(struct mg_http_message *user_response)
{
	g_user_json = (struct s_json *) calloc(sizeof(struct s_json), 1);

	initialize_s_json(g_user_json);

	if (parse_body_json(user_response->body, g_user_json))
	{
		if (create_s_json_string(g_user_json, &g_json_string))
		{
			destroy_s_json(g_user_json);
			return (1);
		}
		destroy_s_json(g_user_json);
	}
	return (0);
}

void	parse_token(struct mg_http_message *response, char *token)
{
	mjson_get_string(response->body.ptr, response->body.len, "$.access_token", token, 64);
}

/* Getters and Setters */
void	set_tls(struct mg_connection *c, struct mg_str *host, char *address)
{
	if (mg_url_is_ssl(address))
	{
		struct mg_tls_opts opts = {.srvname = *host};
		mg_tls_init(c, &opts);
	}
}

void	set_response_status(struct mg_http_message *response, char *status)
{
	sprintf(status, "%.*s", (int)response->uri.len, response->uri.ptr);
}

char	*get_user(struct mg_http_message *request)
{
	char *user;

	user = (char *)calloc(sizeof(char), request->uri.len - 7);
	sprintf(user, "%.*s", (int) request->uri.len - 8, request->uri.ptr + 8);
	return (user);
}

int		get_method(struct mg_http_message *request)
{
	if (!mg_ncasecmp(request->method.ptr, "GET", request->method.len))
		return (GET);
	else if (!mg_ncasecmp(request->method.ptr, "PUT", request->method.len))
		return (PUT);
	else if (!mg_ncasecmp(request->method.ptr, "POST", request->method.len))
		return (POST);
	else if (!mg_ncasecmp(request->method.ptr, "DELETE", request->method.len))
		return (DELETE);
	else if (!mg_ncasecmp(request->method.ptr, "OPTIONS", request->method.len))
		return (OPTIONS);
	else if (!mg_ncasecmp(request->method.ptr, "HEAD", request->method.len))
		return (HEAD);
	return (-1);
}

int		get_uri(struct mg_http_message *request)
{
	if (mg_http_match_uri(request, "/"))
		return (HOME_ROUTE);
	else if (mg_http_match_uri(request, "/docs"))
		return (DOCS_ROUTE);
	else if (mg_http_match_uri(request, "/docs/"))
		return (DOCS_ROUTE_);
	else if (mg_http_match_uri(request, "/api/v1/"))
		return (API_BASE_ROUTE);
	else if (mg_http_match_uri(request, "/api/v1"))
		return (API_BASE_ROUTE_);
	else if (mg_http_match_uri(request, "/api/v1/*"))
		return (API_MAIN_ROUTE);
	return (-1);
}
