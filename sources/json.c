#include "enki.h"

/* s_json methods */
int		parse_body_json(struct mg_str body, struct s_json *json)
{
	json->intra_id			= get_simple_item(body, ID_PATH);
	json->login				= get_simple_item(body, LOGIN_PATH);
	json->name				= get_simple_item(body, NAME_PATH);
	json->staff				= get_simple_item(body, BOCAL_PATH); 
	json->correction_p		= get_simple_item(body, COREP_PATH);
	json->wallet			= get_simple_item(body, WALLET_PATH);
	json->first_day			= get_simple_item(body, FDAY_PATH);
	json->level_42			= get_42_level(body);
	json->specialization	= get_specialization(body);
	json->projects			= get_projects(body);
	json->retries			= get_retries(body);
	if (check_s_json_integrity(json))
	{
		set_s_json_len(json);
		return (1);
	}
	destroy_s_json(json);
	return (0);
}

void	set_s_json_len(struct s_json *json)
{
	json->len += strlen(json->intra_id);
	json->len += strlen(json->login);
	json->len += strlen(json->name);
	json->len += strlen(json->staff);
	json->len += strlen(json->specialization);
	json->len += strlen(json->correction_p);
	json->len += strlen(json->wallet);
	json->len += strlen(json->projects);
	json->len += strlen(json->first_day);
	json->len += strlen(json->retries);
	json->len += strlen(json->level_42);
}

void	initialize_s_json(struct s_json *json)
{
	json->intra_id			= NULL;
	json->login				= NULL;
	json->name 				= NULL;
	json->staff 			= NULL;
	json->specialization	= NULL;
	json->correction_p		= NULL;
	json->wallet			= NULL;
	json->projects			= NULL;
	json->first_day			= NULL;
	json->retries			= NULL;
	json->level_42			= NULL;
}

void	destroy_s_json(struct s_json *json)
{
	free(json->intra_id);
	free(json->login);
	free(json->name);
	free(json->staff);
	free(json->specialization);
	free(json->correction_p);
	free(json->wallet);
	free(json->projects);
	free(json->first_day);
	free(json->retries);
	free(json->level_42);
	free(json);
	json = NULL;
}

int		check_s_json_integrity(struct s_json *json)
{
	if (!json)					return (0);
	if (!json->intra_id)		return (0);
	if (!json->login)			return (0);
	if (!json->name)			return (0);
	if (!json->staff)			return (0);
	if (!json->specialization)	return (0);
	if (!json->correction_p)	return (0);
	if (!json->wallet)			return (0);
	if (!json->projects)		return (0);
	if (!json->first_day)		return (0);
	if (!json->retries)			return (0);
	if (!json->level_42)		return (0);
	return (1);
}

int		create_s_json_string(struct s_json *json, char **json_string)
{
	*json_string = (char *) calloc(sizeof(char), json->len + 250);
	if (!*json_string)
		return (0);

	mjson_snprintf(*json_string,
				   json->len + 250,
				   "{%Q: %d, %Q: %s, %Q: %s, %Q: %s, %Q: %B, %Q: %s, %Q: %d, %Q: %d, %Q: %s, %Q: %d, %Q: %.*g, %Q: %d}",
				   "intra_id", atoi(json->intra_id),
				   "login", json->login,
				   "name", json->name,
				   "bocal", json->staff,
				   "specialization", atoi(json->specialization),
				   "correction_points", json->correction_p,
				   "wallet", atoi(json->wallet),
				   "concluded_projects", atoi(json->projects),
				   "day_started_on_42", json->first_day,
				   "days_wasted_on_retries", atoi(json->retries),
				   "stardew_valley_coeficient", 2, (double)atoi(json->wallet)/350,
				   "42_level", atoi(json->level_42)
				   );
	return (1);
}

/* Getters */
char	*get_simple_item(struct mg_str body, char *json_path)
{
	const char	*tmp;
	char		*ans;
	int			ans_len;

	ans = NULL;
	mjson_find(body.ptr, body.len, json_path, &tmp, &ans_len);
	ans = (char *) calloc(sizeof(char), ans_len + 1);
	if (ans)
		sprintf(ans, "%.*s", ans_len, tmp);
	return (ans);
}

char	*get_specialization(struct mg_str body)
{
	static char	query[35];
	const char	*tmp;
	char		*ans;
	int			tmp_len;
	int			i = 0;

	ans = NULL;
	sprintf(query, "$.projects_users[%d].project.id", i);
	while (mjson_find(body.ptr, (int)body.len, query, &tmp, &tmp_len))
	{
		if (tmp_len == 4)
			if (!strncmp(tmp, "1337", tmp_len))
			{
				ans = (char *) calloc(sizeof(char), 2);
				if (ans)
					sprintf(ans, "%d", 1);
				return (ans);
			}
		sprintf(query, "$.projects_users[%d].project.id", ++i);
	}
	ans = (char *) calloc(sizeof(char), 2);
	if (ans)
		sprintf(ans, "%d", 0);
	return (ans);
}

char	*get_projects(struct mg_str body)
{
	static char	query[35];
	const char	*tmp;
	char		*ans;
	int			i, j, tmp_len;

	i = 0;
	j = 0;
	ans = NULL;
	sprintf(query, "$.projects_users[%d].status", i);
	while (mjson_find(body.ptr, (int)body.len, query, &tmp, &tmp_len))
	{
		if (tmp_len == 10)
			if (!strncmp(tmp, "\"finished\"", tmp_len))
				j++;
		sprintf(query, "$.projects_users[%d].status", ++i);
	}
	ans = (char *) calloc(sizeof(char), 4);
	if (ans)
		sprintf(ans, "%d", j);
	return (ans);
}

char	*get_retries(struct mg_str body)
{
	static char	query[35];
	const char	*tmp;
	char		*ans;
	int			i, j, tmp_len;

	i = 0;
	j = 0;
	ans = NULL;
	sprintf(query, "$.projects_users[%d].occurrence", i);
	while (mjson_find(body.ptr, (int)body.len, query, &tmp, &tmp_len))
	{
		j += atoi(tmp);
		sprintf(query, "$.projects_users[%d].occurrence", ++i);
	}
	ans = (char *) calloc(sizeof(char), 4);
	if (ans)
		sprintf(ans, "%d", j);
	return (ans);
}

char	*get_42_level(struct mg_str body)
{
	double		tmp;
	char		*ans;

	ans = NULL;
	mjson_get_number(body.ptr, body.len, "$.id", &tmp);
	tmp = (abs((int)(((((((((tmp + 42) - 8) * 4) / 30) * tmp) / 70) + 400) - 700) / 5)) % 43);
	ans = (char *) calloc(sizeof(char), 3);
	if (ans)
		sprintf(ans, "%d", (int)tmp);
	return (ans);
}
