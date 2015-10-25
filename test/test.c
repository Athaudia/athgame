#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <athgame.h>
#include <age_famicom.h>
#include <age_ehbasic.h>
//#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

char* load_file(char* fname)
{
	FILE* fil = fopen(fname, "r");
	fseek(fil, 0, SEEK_END);
	size_t len = ftell(fil);
	rewind(fil);
	char* data = (char*)malloc(sizeof(char)*len);
	fread(data, 1, len, fil);
	return data;
}

enum token_type
{
	TOKEN_UNDEFINED, TOKEN_SYMBOL, TOKEN_QUOTED_SYMBOL, TOKEN_PAR_OPEN, TOKEN_PAR_CLOSE, TOKEN_BRACKET_OPEN, TOKEN_BRACKET_CLOSE, TOKEN_NUMBER
};

struct token
{
	enum token_type type;
	union
	{
		int num_int;
		char* str;
	};
	struct token* next;
};

struct token* token_new()
{
	struct token* token = (struct token*)malloc(sizeof(struct token));
	token->type = TOKEN_UNDEFINED;
	token->next = 0;
	return token;
}

bool is_whitespace(char ch)
{
	return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n';
}

bool is_digit(char ch)
{
	return ch>='0' && ch<='9';
}

bool is_letter(char ch)
{
	return (ch>='a' && ch<='z') || (ch>='A' && ch <= 'Z');
}

bool is_legal_symbol_first_char(char ch)
{
	return is_letter(ch) || ch=='!' || ch=='#' || ch=='$' || ch=='%' || ch=='&' || ch=='*' || ch=='+' || ch==',' || ch=='-' || ch=='.' ||
		ch=='/' || ch==':' || ch=='<' || ch=='=' || ch=='>' || ch=='?' || ch=='@' || ch=='\\' || ch=='^' || ch=='_' || ch=='|' || ch=='~';
}

bool is_legal_symbol_char(char ch)
{
	return is_legal_symbol_first_char(ch) || is_digit(ch);
}

struct token* tokenize(char* source)
{
	struct token* first = token_new();
	struct token* current = first;
	char strbuf[256];
	memset(strbuf, 0, 256);
	char* cur_str = strbuf;
	while(*source)
	{
		bool finish = false;
		switch(current->type)
		{
		case TOKEN_UNDEFINED:
			if(is_whitespace(*source))
				break;

			switch(*source)
			{
			case '(': current->type = TOKEN_PAR_OPEN;      finish = true; break;
			case ')': current->type = TOKEN_PAR_CLOSE;     finish = true; break;
			case '[': current->type = TOKEN_BRACKET_OPEN;  finish = true; break;
			case ']': current->type = TOKEN_BRACKET_CLOSE; finish = true; break;

			case '\'':
				current->type = TOKEN_QUOTED_SYMBOL;
				break;
			default:
				if(is_whitespace(*source))
				   break;
				if(is_legal_symbol_first_char(*source))
				{
					current->type = TOKEN_SYMBOL;
					--source;
				}
				else
				{
					printf("error: illegal character '%c' %i\n", *source, *source);
					return first;
				}
				break;
			}
			break;

		case TOKEN_SYMBOL:
		case TOKEN_QUOTED_SYMBOL:
		{
			if(cur_str == strbuf) //first char
			{
				if(is_legal_symbol_first_char(*source))
					*(cur_str++) = *source;
				else
					finish = true;
			}
			else
			{
				if(is_legal_symbol_char(*source))
					*(cur_str++) = *source;
				else
					finish = true;
			}
			if(finish)
				--source;
			break;
		}
		default:
			break;
		}

		if(finish)
		{
			current->str = strdup(strbuf);
			memset(strbuf, 0, 256);
			cur_str = strbuf;
			struct token* next = token_new();
			current->next = next;
			current = next;
		}

		++source;
	}
	return first;
}


int main()
{
	/*	struct token* tok = tokenize(load_file("../min.ath"));
	while(tok)
	{
		printf("%i %s\n", tok->type, tok->str);
		tok = tok->next;
		}*/
	//struct ag_tcp_con_buffered* con = ag_tcp_con_buffered_new("google.com", "80", 512, "\r\n");
	//struct ag_irc_msg* msg = ag_irc_msg_new_from_string("command a b :a u i");

	struct ag_irc_client* client = ag_irc_client_new("irc.esper.net", "6667", "athbot");

	while(true)
	{
		struct ag_irc_msg* msg = ag_irc_client_recv(client);
		if(msg)
			printf("%s\n", ag_irc_msg_debug_inspect(msg));
		else
			ag_sleep(10);
	}
	ag_irc_client_destroy(client);
	return 0;
}
