#include "inf.h"

/**
 * inf_is_chain - test if current char in buffer is a chain delimeter
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 *
 * Return: 1 if chain delimeter, 0 otherwise
 */
int inf_is_chain(inf_t *info, char *buf, size_t *p)
{
        size_t j = *p;

        if (buf[j] == '|' && buf[j + 1] == '|')
        {
                buf[j] = 0;
                j++;
                info->cmd_buf_type = INF_CMD_OR;
        }
        else if (buf[j] == '&' && buf[j + 1] == '&')
        {
                buf[j] = 0;
                j++;
                info->cmd_buf_type = INF_CMD_AND;
        }
        else if (buf[j] == ';') /* found end of this command */
        {
                buf[j] = 0; /* replace semicolon with null */
                info->cmd_buf_type = INF_CMD_CHAIN;
        }
        else
                return (0);
        *p = j;
        return (1);
}

/**
 * inf_check_chain - checks we should continue chaining based on last status
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 * @i: starting position in buf
 * @len: length of buf
 *
 * Return: Void
 */
void inf_check_chain(inf_t *info, char *buf, size_t *p, size_t i, size_t len)
{
        size_t j = *p;

        if (info->cmd_buf_type == INF_CMD_AND)
        {
                if (info->status)
                {
                        buf[i] = 0;
                        j = len;
                }
        }
        if (info->cmd_buf_type == INF_CMD_OR)
        {
                if (!info->status)
                {
                        buf[i] = 0;
                        j = len;
                }
        }

        *p = j;
}

/**
 * inf_replace_alias - replaces an aliases in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int inf_replace_alias(inf_t *info)
{
        int i;
        inf_list_t *node;
        char *p;

        for (i = 0; i < 10; i++)
        {
                node = inf_node_starts_with(info->alias, info->argv[0], '=');
                if (!node)
                        return (0);
                free(info->argv[0]);
                p = inf_strchr(node->str, '=');
                if (!p)
                        return (0);
                p = inf_strdup(p + 1);
                if (!p)
                        return (0);
                info->argv[0] = p;
        }
        return (1);
}

/**
 * inf_replace_vars - replaces vars in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int inf_replace_vars(inf_t *info)
{
        int i = 0;
        inf_list_t *node;

        for (i = 0; info->argv[i]; i++)
        {
                if (info->argv[i][0] != '$' || !info->argv[i][1])
                        continue;

                if (!inf_strcmp(info->argv[i], "$?"))
                {
                        inf_replace_string(&(info->argv[i]),
                                        inf_strdup(inf_convert_number(info->status, 10, 0)));
                        continue;
                }
                if (!inf_strcmp(info->argv[i], "$$"))
                {
                        inf_replace_string(&(info->argv[i]),
                                        inf_strdup(inf_convert_number(getpid(), 10, 0)));
                        continue;
                }
                node = inf_node_starts_with(info->env, &info->argv[i][1], '=');
                if (node)
                {
                        inf_replace_string(&(info->argv[i]),
                                        inf_strdup(inf_strchr(node->str, '=') + 1));
                        continue;
                }
                inf_replace_string(&info->argv[i], inf_strdup(""));

        }
        return (0);
}

/**
 * inf_replace_string - replaces string
 * @old: address of old string
 * @new: new string
 *
 * Return: 1 if replaced, 0 otherwise
 */
int inf_replace_string(char **old, char *new)
{
        free(*old);
        *old = new;
        return (1);
}
