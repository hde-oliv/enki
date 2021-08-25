#ifndef ENKI_H
# define ENKI_H

/************************************************
 *  Includes Section
 ***********************************************/
# include <mongoc/mongoc.h>
# include <stdlib.h>
# include "mongoose.h"
# include "mjson.h"

/************************************************
 *  Defines Section
 ***********************************************/
# define DOCS_FILE "./assets/index.html"
# define TOKEN_ADDRESS "https://api.intra.42.fr/oauth/token"
# define INTRA_ADDRESS "https://api.intra.42.fr/v2/users"
# define ID_PATH "$.id"
# define LOGIN_PATH "$.login"
# define NAME_PATH "$.displayname"
# define BOCAL_PATH "$.staff\\?"
# define COREP_PATH "$.correction_point"
# define WALLET_PATH "$.wallet"
# define FDAY_PATH "$.created_at"
# define SECRET_PATH "$.id"

/************************************************
 *  Structs Section
 ***********************************************/
struct s_json
{
	char	*intra_id;
	char	*login;
	char	*name;
	char	*staff;
	char	*specialization;
	char	*correction_p;
	char	*wallet;
	char	*projects;
	char	*first_day;
	char	*retries;
	char	*level_42;
	int		len;
};

/************************************************
 *  Enums Section
 ***********************************************/
enum e_methods
{
	GET,
	POST,
	PUT,
	PATCH,
	DELETE,
	OPTIONS,
	HEAD
};

enum e_uri
{
	HOME_ROUTE,
	DOCS_ROUTE,
	DOCS_ROUTE_,
	API_BASE_ROUTE,
	API_BASE_ROUTE_,
	API_MAIN_ROUTE,
};

/************************************************
 *  Api.c Functions Section
 ***********************************************/

/**
 * @brief It is the the first function to be function
 * called, initializes Mongoose and MongoDB connection.
 *
 * @param mgr The main manager struct (global).
 */
void	start_api(struct mg_mgr *mgr);

/**
 * @brief Simply starts the server and start
 * the connection on API_ADDRESS.
 * 
 * @param mgr The main manager struct (global).
 * @param callback The callback use on poll.
 */
void	handle_api(struct mg_mgr *mgr, mg_event_handler_t callback);

/**
 * @brief Gracefully destroys everything and
 * end all connections.
 *
 * @param mgr The main manager struct (global).
 */
void	end_api(struct mg_mgr *mgr);

/**
 * @brief Registers the main callback on poll,
 * it is called by handle_api.
 *
 * @param mgr The main manager struct (global).
 * @param callback The callback use on poll (enki_cb).
 */
void	enki_listen(struct mg_mgr *mgr, mg_event_handler_t callback);

/**
 * @brief Starts the Intra Token requisition and
 * exits when its lock is liberated.
 *
 * @param mgr The main manager struct (global).
 * @param callback The callback use on poll (token_cb).
 */
void	intra_token(struct mg_mgr *mgr, mg_event_handler_t callback);

/**
 * @brief Starts the Intra User requisition and
 * exits when its lock is liberated.
 *
 * @param mgr The main manager struct (global).
 * @param callback The callback use on poll (user_cb).
 */
void	intra_user(struct mg_mgr *mgr, mg_event_handler_t callback);


/**
 * @brief This function is called on start_api
 * and listens to signals to see if SIGTERM is
 * called.
 */
void	check_sigterm(void);

/**
 * @brief The function registered on check_sigterm,
 * sets the global variable done to 1, so end_api
 * enters on call stack.
 *
 * @param signum The signum the system sent.
 */
void	handle_sigterm(int signum);

/**
 * @brief A wrapper to set the is_closing field
 * of the passed connection to 1, to end it. Also
 * sets the callback lock to 1 to end it too.
 *
 * @param c The main connection pointer.
 * @param lock The callback lock pointer.
 */
void	close_connection(struct mg_connection *c, int *lock);

/************************************************
 *  Callback.c Functions Section
 ***********************************************/

/**
 * @brief The main callback it listens to
 * all requests on API_ADDRESS and call
 * handle_routes to handle them.
 *
 * @param c The main connection pointer.
 * @param ev The event state.
 * @param ev_data The event data.
 * @param fn_data The lock pointer.
 */
void	enki_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

/**
 * @brief The token callback to call the TOKEN_ADDRESS
 * and retrieve the token to access INTRA_ADDRESS.
 *
 * @param c The main connection pointer.
 * @param ev The event state.
 * @param ev_data The event data.
 * @param fn_data The lock pointer.
 */
void	token_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

/**
 * @brief The user callback to call INTRA_ADDRESS
 * and get the user info.
 *
 * @param c The main connection pointer.
 * @param ev The event state.
 * @param ev_data The event data.
 * @param fn_data The lock pointer.
 */
void	user_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

/************************************************
 *  Router.c Functions Section
 ***********************************************/

/**
 * @brief Handles all routes requests and responds accordingly.
 *
 * @param c The main connection pointer.
 * @param request A pointer to the request received.
 */
void	handle_routes(struct mg_connection *c, struct mg_http_message *request);

/**
 * @brief The user callback to call INTRA_ADDRESS
 * and get the user info.
 *
 * @param c The main connection pointer.
 * @param request A pointer to the request received.
 */
void	handle_main_route(struct mg_connection *c, struct mg_http_message *request);

/**
 * @brief A wrapper to respond to create the main route
 * response.
 *
 * @param user_reponse A pointer to the response of user callback.
 * @return 1 on success, 0 on failure.
 */
int		create_main_route_response(struct mg_http_message *user_response);

/**
 * @brief A function to extract the token from the response of
 * the call to TOKEN_ADDRESS. It writes on the passed char pointer.
 *
 * @param response A pointer to the reponse of token callback.
 * @token token A pointer to write the token on. (Must have 65 bytes at least)
 */
void	parse_token(struct mg_http_message *response, char *token);

/**
 * @brief A function to extract the user from the URI on the
 * main route request.
 *
 * @param request A pointer to the main callback request.
 * @return char* A allocated user string.
 */
char	*get_user(struct mg_http_message *request);

/**
 * @brief A function to extract the method from a request.
 *
 * @param request A pointer to the main callback request.
 * @return int The enum equivalent or -1 on invalid method.
 */
int		get_method(struct mg_http_message *request);

/**
 * @brief A function to extract the URI from a request.
 *
 * @param request A pointer to the main callback request.
 * @return int The enum equivalent or -1 on invalid URI.
 */int		get_uri(struct mg_http_message *request);

/**
 * @brief A wrapper to set TLS on TOKEN_ADDRESS and INTRA_ADDRESS
 * requests.
 *
 * @param c The main connection pointer.
 * @param host The host string (mg_str).
 * @param address The address on where to set the connection string.
 */
void	set_tls(struct mg_connection *c, struct mg_str *host, char *address);

/**
 * @brief A wrapper to get the status of a response and sets it to a string.
 * requests.
 *
 * @param response The reponse pointer.
 * @param status The string where to write on.
 */
void	set_response_status(struct mg_http_message *response, char *status);


/************************************************
 *  Request.c Functions Section
 ***********************************************/

/**
 * @brief A wrapper to write (send) the request on the connection
 * to TOKEN_ADDRESS.
 *
 * @param c The main connection pointer.
 * @param host The host string (mg_str).
 */
void	send_token_request(struct mg_connection *c, struct mg_str *host);

/**
 * @brief A wrapper to write (send) the request on the connection
 * to INTRA_ADDRESS.
 *
 * @param c The main connection pointer.
 * @param host The host string (mg_str).
 * @param user The user string.
 * @param token The token string.
 */
void	send_user_info_request(struct mg_connection *c, struct mg_str *host, char *user, char *token);

/************************************************
 *  Json.c Functions Section
 ***********************************************/

/**
 * @brief A wrapper to parse the response json (from user callback),
 * because of that it can be interpreted as the s_json constructor.
 *
 * @param body The body of the response.
 * @param json The pointer s_json to write on.
 * @returns int 1 on success, 0 on failure.
 */
int		parse_body_json(struct mg_str body, struct s_json *json);

/**
 * @brief A wrapper to set the len field of s_json. It is
 * the len of all fields combined.
 *
 * @param json The pointer s_json to write on.
 */
void	set_s_json_len(struct s_json *json);

/**
 * @brief A wrapper to free the s_json, can be interpreted
 * as a destructor.
 *
 * @param json The pointer s_json free everything.
 */
void	destroy_s_json(struct s_json *json);

/**
 * @brief A wrapper to check if all fields of a s_json
 * have been properly allocated.
 *
 * @param json The pointer s_json to write on.
 * @returns int 1 if everything is fine, 0 if encounters a NULL pointer.
 */
int		check_s_json_integrity(struct s_json *json);

/**
 * @brief A wrapper to create a string of the s_json struct.
 * It allocates memory on the pointer given.
 *
 * @param json The pointer s_json to write on.
 * @param json_string A address of a pointer to write on.
 * @returns int 1 on success, 0 on failure.
 */
int		create_s_json_string(struct s_json *json, char **json_string);

/**
 * @brief A wrapper to initialize the s_json, setting
 * everything to NULL.
 *
 * @param json The pointer s_json to write on.
 */
void	initialize_s_json(struct s_json *json);

/**
 * @brief A generic getter to fetch a single item from
 * the body response.
 *
 * @param body The body of the response.
 * @param json_path The json path to search on.
 * @returns char * A allocated string of the result.
 */
char	*get_simple_item(struct mg_str body, char *json_path);

/**
 * @brief A generic getter to fetch a single item from
 * the body response.
 *
 * @param body The body of the response.
 * @param json_path The json path to search on.
 * @returns char * A allocated string of the result.
 */

char	*get_specialization(struct mg_str body);

/**
 * @brief A wrapper to get the number of retries of the
 * user.
 *
 * @param body The body of the response.
 * @returns char * A allocated string of the result.
 */
char	*get_retries(struct mg_str body);

/**
 * @brief A wrapper to get the number of finished
 * projects of a user.
 *
 * @param body The body of the response.
 * @returns char * A allocated string of the result.
 */
char	*get_projects(struct mg_str body);

/**
 * @brief A wrapper to get the number of the 42 level
 * of the user.
 *
 * @param body The body of the response.
 * @returns char * A allocated string of the result.
 */
char	*get_42_level(struct mg_str body);


/************************************************
 *  Mongo.c Functions Section
 ***********************************************/

/**
 * @brief A wrapper to all database actions.
 *
 * @param json A json string to update or record.
 */
void	handle_database_crud(const char *json);

/**
 * @brief A wrapper to start the database connection.
 *
 * @param client A pointer address to the client.
 * @param database A pointer address to the database.
 * @param collection A pointer address to the collection.
 */
void	start_connection(mongoc_client_t **client, mongoc_database_t **database, mongoc_collection_t **collection);

/**
 * @brief A wrapper to end the database connection.
 *
 * @param client A pointer address to the client.
 * @param database A pointer address to the database.
 * @param collection A pointer address to the collection.
 */
void	end_connection(mongoc_client_t **client, mongoc_database_t **database, mongoc_collection_t **collection);

/**
 * @brief A function to insert the passed bson string on
 * the database.
 *
 * @param collection A pointer address to the collection.
 * @param bson A pointer address to the bson string to insert.
 * @param error A pointer to the error struct to write errors.
 */
void	create_operation(mongoc_collection_t **collection, bson_t **bson, bson_error_t *error);

/**
 * @brief Checks if the user is already on the database,
 * if it is, writes its oid on the passed struct.
 *
 * @param collection A pointer address to the collection.
 * @param oid A pointer on to write the oid.
 * @returns int 1 if exists, 0 if not.
 */
int		read_operation(mongoc_collection_t **collection, bson_oid_t *oid);

/**
 * @brief A function to update the user information on
 * the database.
 *
 * @param collection A pointer address to the collection.
 * @param bson A pointer address to the bson string to insert.
 * @param error A pointer to the error struct to write errors.
 * @param oid A pointer on to the oid to search for.
 */
void	update_operation(mongoc_collection_t **collection, bson_t **bson, bson_error_t *error, const bson_oid_t *oid);

#endif
