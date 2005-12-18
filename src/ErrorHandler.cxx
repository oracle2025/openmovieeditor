#define ERROR( x ) 


errorDialog( "Folgender Fehler ist passiert: %s\n", "genaue Fehlermeldung" );

string h;
h.printf()


void errorDialog( const char* s, ... )
{
	va_list zeiger;
	va_start( zeiger, s );
	gchar* msg = g_strdup_vprintf( s, zeiger );
	cerr << msg << endl;
	// add idle handler to popup dialog.
}
