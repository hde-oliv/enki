#include "enki.h"
#include "mongoose.h"

extern struct mg_mgr	g_mgr;
static int				done;
static int				token_lock;
static int				intra_lock;

/* Main API functions */
void	start_api(struct mg_mgr *mgr)
{
	mg_mgr_init(mgr);
	mongoc_init();
}

void	handle_api(struct mg_mgr *mgr, mg_event_handler_t callback)
{
	enki_listen(mgr, callback);
}

void	end_api(struct mg_mgr *mgr)
{
	mongoc_cleanup();
	mg_mgr_free(mgr);
}

/* Graceful shutdown handling */
void	handle_sigterm(int signum)
{
	(void)signum;
	done = 1;
	exit(0);
}

void	check_sigterm(void)
{
	static struct sigaction	sigact;

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler = handle_sigterm;
}

/* Events */
void	enki_listen(struct mg_mgr *mgr, mg_event_handler_t callback)
{
	char	api_address[20];
	char	*port = getenv("PORT");

	sprintf(api_address, "http://0.0.0.0:%s", port);
	mg_http_listen(mgr, api_address, callback, NULL);
	while (!done) mg_mgr_poll(mgr, 1000);
}

void	intra_token(struct mg_mgr *mgr, mg_event_handler_t callback)
{
	mg_http_connect(mgr, TOKEN_ADDRESS, callback, &token_lock);
	while (true && !token_lock) mg_mgr_poll(mgr, 1000);
	token_lock = 0;
}

void	intra_user(struct mg_mgr *mgr, mg_event_handler_t callback)
{
	mg_http_connect(mgr, INTRA_ADDRESS, callback, &intra_lock);
	while (true && !intra_lock) mg_mgr_poll(mgr, 1000);
	intra_lock = 0;
}

/* Utilities */
void	close_connection(struct mg_connection *c, int *lock)
{
	c->is_closing = 1;
	*lock = 1;
}
