#include "enki.h"

struct mg_mgr	g_mgr;

int	main(void)
{
	start_api(&g_mgr);
	handle_api(&g_mgr, enki_cb);
	end_api(&g_mgr);
	return (0);
}
