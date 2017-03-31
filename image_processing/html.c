#include <stdio.h>

static FILE* html;

void html_init(){
	html = fopen( "index.html", "w" );
	
	fprintf( html, "<html>\n" );
	fprintf( html, "<head>\n" );
	fprintf( html, "<title>Result of some image processing filters</title>\n");
	fprintf( html, "</head>\n" );
	fprintf( html, "<body>\n" );
	fprintf( html, "<div id='container'>\n" );
	fprintf( html, "<ol>\n" );
}

void html_print( char* filtername, char* resultfile ){

	fprintf( html, "\t<li>\n" );
	fprintf( html, "\t\t<h3>%s</h3>\n", filtername );
	fprintf( html, "\t\t<img src='%s' />\n", resultfile );
	fprintf( html, "\t</li>\n" );
}

void destroy_html(){
	fprintf( html, "</ol>\n" );
	fprintf( html, "</body\n" );
	fprintf( html, "</html>\n" );

	fclose(html);
	html = NULL;
}
