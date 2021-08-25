#include "enki.h"

void	send_token_request(struct mg_connection *c, struct mg_str *host)
{
	char	*client_id		=	getenv("CLIENT_ID");
	char	*client_secret	=	getenv("CLIENT_SECRET");

	mg_printf(
		c,
		"POST %s HTTP/1.0\r\n"
		"Host: %.*s\r\n"
		"Accept: */*\r\n"
		"Content-Length: 183\r\n"
		"\r\n"
		"grant_type=client_credentials&"
		"client_id=%s&"
		"client_secret=%s"
		"\r\n",
		mg_url_uri(TOKEN_ADDRESS),
		(int)host->len,
		host->ptr,
		client_id,
		client_secret);
}

void	send_user_info_request(struct mg_connection *c, struct mg_str *host, char *user, char *token)
{
	mg_printf(
		c,
		"GET %s/%s HTTP/1.0\r\n"
		"Host: %.*s\r\n"
		"Authorization: Bearer %s\r\n"
		"Accept: */*\r\n"
		"\r\n",
		mg_url_uri(INTRA_ADDRESS),
		user,
		(int)host->len,
		host->ptr,
		token);
}
