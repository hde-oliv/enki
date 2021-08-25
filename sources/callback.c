#include "enki.h"
#include "mongoose.h"

extern char	g_token[65];
extern char	g_status[4];
extern char *g_user;

void	enki_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	struct mg_http_message	*request	=	(struct mg_http_message *)ev_data;

	if (ev == MG_EV_HTTP_MSG)
		handle_routes(c, request);
	(void)fn_data;
}

void	token_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	struct mg_str			host		= mg_url_host(TOKEN_ADDRESS);
	struct mg_http_message	*response	= (struct mg_http_message *)ev_data;

	if (ev == MG_EV_CONNECT)
	{
		set_tls(c, &host, TOKEN_ADDRESS);
		send_token_request(c, &host);
	}
	else if (ev == MG_EV_HTTP_MSG)
	{
		set_response_status(response, g_status);
		if (!strcmp(g_status, "200"))
		{
			parse_token(response, g_token);
			close_connection(c, fn_data);
		}
		close_connection(c, fn_data);
	}
	else if (ev == MG_EV_ERROR)
	{
		sprintf(g_status, "500");
		close_connection(c, fn_data);
	}
}

void	user_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	struct mg_str			host		=	mg_url_host(INTRA_ADDRESS);
	struct mg_http_message	*response	=	(struct mg_http_message *)ev_data;

	if (ev == MG_EV_CONNECT)
	{
		set_tls(c, &host, INTRA_ADDRESS);
		send_user_info_request(c, &host, g_user, g_token);
	}
	else if (ev == MG_EV_HTTP_MSG)
	{
		set_response_status(response, g_status);
		if (!strcmp(g_status, "200"))
			if (!create_main_route_response(response))
				sprintf(g_status, "500");
		close_connection(c, fn_data);
	}
	else if (ev == MG_EV_ERROR)
	{
		sprintf(g_status, "500");
		close_connection(c, fn_data);
	}
}
