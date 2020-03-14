http_server: http_server
	gcc main.c response_builder.c config_parser.c -std=gnu99 -o http_server
