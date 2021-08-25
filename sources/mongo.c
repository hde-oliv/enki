#include "enki.h"

extern char	*g_user;

void	handle_database_crud(const char *json)
{
	mongoc_client_t			*client;
	mongoc_database_t		*database;
	mongoc_collection_t		*collection;
	bson_t      			*bson;
	bson_error_t 			error;
	bson_oid_t				oid;

	start_connection(&client, &database, &collection);

	bson = bson_new_from_json((const uint8_t *)json, -1, &error);
	if (!read_operation(&collection, &oid))
		create_operation(&collection, &bson, &error);
	else
		update_operation(&collection, &bson, &error, &oid);
	bson_destroy(bson);
	end_connection(&client, &database, &collection);
}

void	start_connection(mongoc_client_t **client, mongoc_database_t **database, mongoc_collection_t **collection)
{
	char	*m_url = getenv("MONGO_CLIENT_URL");
	
	*client = mongoc_client_new(m_url);
	mongoc_client_set_appname(*client, "Enki API");
	*database = mongoc_client_get_database(*client, "enki_api");
	*collection = mongoc_client_get_collection(*client, "enki_api", "enki_api");
}

void	end_connection(mongoc_client_t **client, mongoc_database_t **database, mongoc_collection_t **collection)
{
	mongoc_collection_destroy(*collection);
	mongoc_database_destroy(*database);
	mongoc_client_destroy(*client);
}

void	create_operation(mongoc_collection_t **collection, bson_t **bson, bson_error_t *error)
{
	mongoc_collection_insert_one(*collection, *bson, NULL, NULL, error);
}

int		read_operation(mongoc_collection_t **collection, bson_oid_t *oid)
{
	mongoc_cursor_t		*cursor;
	const bson_t		*doc;
	bson_t				*query;
	bson_iter_t			iter;
	bson_iter_t			id;
	const bson_oid_t	*tmp;
	
	query = bson_new();
	BSON_APPEND_UTF8(query, "login", g_user);
	cursor = mongoc_collection_find_with_opts(*collection, query, NULL, NULL);
	if (mongoc_cursor_next(cursor, &doc))
		if (bson_iter_init(&iter, doc) && bson_iter_find_descendant(&iter, "_id", &id) && BSON_ITER_HOLDS_OID(&id))
		{
			tmp = bson_iter_oid(&id);
			bson_oid_copy(tmp, oid);
			bson_destroy(query);
			mongoc_cursor_destroy(cursor);
			return (1);
		}
	bson_destroy(query);
	mongoc_cursor_destroy(cursor);
	return (0);
}

void	update_operation(mongoc_collection_t **collection, bson_t **bson, bson_error_t *error, const bson_oid_t *oid)
{
	bson_t	*query = NULL;
	bson_t	update;

	query = BCON_NEW("_id", BCON_OID(oid));

	bson_init(&update);
	bson_append_document(&update, "$set", 4, *bson);

	mongoc_collection_update_one(*collection, query, &update, NULL, NULL, error);

	bson_destroy(query);
	bson_destroy(&update);
}
