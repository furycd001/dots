#include "ktar.h"
#include <stdio.h>
#include <kinstance.h>
#include <qfile.h>

void recursive_print( const KTarDirectory * dir, const QString & path )
{
  QStringList l = dir->entries();
  QStringList::Iterator it = l.begin();
  for( ; it != l.end(); ++it )
  {
    const KTarEntry* entry = dir->entry( (*it) );
    printf("mode=%07o %s %s %s%s isdir=%d\n", entry->permissions(), entry->user().latin1(), entry->group().latin1(), path.latin1(), (*it).latin1(), entry->isDirectory());
    if (entry->isDirectory())
      recursive_print( (KTarDirectory *)entry, path+(*it)+"/" );
  }
}

int main( int argc, char** argv )
{
  if (argc != 3)
  {
    printf("\n"
 " Usage :\n"
 " ./ktartest list /path/to/existing_file.tar.gz       tests listing an existing tar.gz\n"
 " ./ktartest readwrite newfile.tar.gz                 will create the tar.gz, then close and reopen it.\n"
 " ./ktartest maxlength newfile.tar.gz                 tests the maximum filename length allowed.\n"
 " ./ktartest bytearray /path/to/existing_file.tar.gz  tests KTarData\n");
    return 1;
  }
  KInstance instance("ktartest");
  QString command = argv[1];
  if ( command == "list" )
  {
    KTarGz tar( argv[2] );

    if ( !tar.open( IO_ReadOnly ) )
    {
      printf("Could not open %s for reading\n", argv[1] );
      return 1;
    }

    const KTarDirectory* dir = tar.directory();

    //printf("calling recursive_print\n");
    recursive_print( dir, "" );
    //printf("recursive_print called\n");

    tar.close();

    return 0;
  }
  else if (command == "readwrite" )
  {
    KTarGz tar( argv[2] );

    if ( !tar.open( IO_WriteOnly ) )
    {
      printf("Could not open %s for writing\n", argv[1]);
      return 1;
    }

    tar.writeFile( "empty", "weis", "users", 0, "" );
    tar.writeFile( "test1", "weis", "users", 5, "Hallo" );
    tar.writeFile( "test2", "weis", "users", 8, "Hallo Du" );
    tar.writeFile( "mydir/test3", "weis", "users", 13, "Noch so einer" );
    tar.writeFile( "my/dir/test3", "dfaure", "hackers", 29, "I don't speak German (David)" );

#define SIZE1 100
    // Now a medium file : 100 null bytes
    char medium[ SIZE1 ];
    memset( medium, 0, SIZE1 );
    tar.writeFile( "mediumfile", "user", "group", SIZE1, medium );
    // Another one, with an absolute path
    tar.writeFile( "/dir/subdir/mediumfile2", "user", "group", SIZE1, medium );

    // Now a huge file : 20000 null bytes
    int n = 20000;
    char * huge = new char[ n ];
    memset( huge, 0, n );
    tar.writeFile( "hugefile", "user", "group", n, huge );
    delete [] huge;

    tar.close();

    printf("-----------------------\n");

    if ( !tar.open( IO_ReadOnly ) )
    {
      printf("Could not open %s for reading\n", argv[1] );
      return 1;
    }

    const KTarDirectory* dir = tar.directory();
    recursive_print(dir, "");

    const KTarEntry* e = dir->entry( "mydir/test3" );
    ASSERT( e && e->isFile() );
    const KTarFile* f = (KTarFile*)e;

    QByteArray arr( f->data() );
    printf("SIZE=%i\n",arr.size() );
    QString str( arr );
    printf("DATA=%s\n", str.latin1());

    tar.close();

    return 0;
  }
  else if ( command == "maxlength" )
  {
    KTarGz tar( argv[2] );

    if ( !tar.open( IO_WriteOnly ) )
    {
      printf("Could not open %s for writing\n", argv[1]);
      return 1;
    }
    // Generate long filenames of each possible length bigger than 98...
    for (int i = 98; i < 500 ; i++ )
    {
      QString str, num;
      str.fill( 'a', i-10 );
      num.setNum( i );
      num = num.rightJustify( 10, '0' );
      tar.writeFile( str+num, "testu", "testg", 3, "hum" );
    }
    // Result of this test : it fails at 482 (instead of 154 previously).
    // Ok, I think we can do with that :)
    tar.close();
    printf("Now run 'tar tvzf %s'\n", argv[2]);
    return 0;
  }
  else if ( command == "bytearray" )
  {
    QFile file( argv[2] );
    if ( !file.open( IO_ReadOnly ) )
      return 1;
    QDataStream stream( &file );
    KTarData tar( &stream );
    tar.open( IO_ReadOnly );
    const KTarDirectory* dir = tar.directory();
    recursive_print( dir, "" );
    return 0;
  }
  else
    printf("Unknown command\n");
}

