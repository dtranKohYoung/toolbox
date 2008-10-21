#include "Savable.h"

				ObjImg::~ObjImg() 
{
	_objImgs.clear();
	if( _el!=NULL ) delete [] _el;
}

void			ObjImg::set( const char *name, const char *type, int n ) 
{ 
	assert( _el==NULL && _objImgs.size()==0 );
	strcpy(_name,name);
	strcpy(_type,type);
	_el			= NULL;
	_elNum		= 0;
	_elBytes	= 0;
	if(n>0) _objImgs.resize(n);
}

void			ObjImg::writeToStrm(ofstream &strm) {
	strm << _type << ' ';
	strm << _name << ' ';
	strm.write((char*)&_elNum,sizeof(_elNum));
	if(_elNum>0) {
		strm.write((char*)&_elBytes,sizeof(_elBytes));
		strm.write(_el,_elNum*_elBytes);
	}
	int n=_objImgs.size(); strm.write((char*)&n,sizeof(n));
	for( int i=0; i<n; i++ ) _objImgs[i].writeToStrm(strm);
}

void			ObjImg::readFrmStrm(ifstream &strm) {
	strm >> _type; strm.get();
	strm >> _name; strm.get();
	strm.read((char*)&_elNum,sizeof(_elNum));
	if(_elNum>0) {
		strm.read((char*)&_elBytes,sizeof(_elBytes));
		_el = new char[_elNum*_elBytes];
		strm.read(_el,_elNum*_elBytes);
	}
	int n; strm.read((char*)&n,sizeof(n)); 
	if(n>0) _objImgs.resize(n);
	for( int i=0; i<n; i++ ) _objImgs[i].readFrmStrm(strm);
}

void			ObjImg::writeToTxtStrm(ofstream &os) {
	os << _type << ' ' << _name << ' ' << _elNum << ' ';
	if(_elNum>0) {
		toStrm( os );
	} else {
		int n=_objImgs.size(); os << n << ' ';
		for( int i=0; i<n; i++ ) _objImgs[i].writeToStrm(os);
	}
}

void			ObjImg::readFrmTxtStrm(ifstream &is) {
	is >> _type; is.get();
	is >> _name; is.get();
	is >> _elNum; is.get();
	if(_elNum>0) {
		is >> _elBytes; is.get();
		//_el = new char[_elNum*_elBytes];
		//is >> *((int*) _el); is.get();
	}
	int n; is >> n; is.get();
	if(n>0) _objImgs.resize(n);
	for( int i=0; i<n; i++ ) _objImgs[i].readFrmStrm(is);
}

bool			ObjImg::saveToFile( const char *fName, bool binary )
{
	ofstream strm; remove( fName );
	strm.open(fName, binary? ios::out|ios::binary : ios::out );
	if (strm.fail()) {
		abortError( "unable to save:", fName, __LINE__, __FILE__ );
		return false;
	}
	if(binary) writeToStrm(strm); else writeToTxtStrm(strm);
	strm.close();
	return true;
}

bool			ObjImg::loadFrmFile( const char *fName, ObjImg &oi, bool binary )
{
	ifstream strm;
	strm.open(fName, binary? ios::in|ios::binary : ios::in );
	if( strm.fail() ) {
		abortError( "unable to load: ", fName, __LINE__, __FILE__ );
		return false;
	}
	if(binary) oi.readFrmStrm(strm); else oi.readFrmTxtStrm(strm);
	strm.close();
	return true;
}

void			ObjImg::toStrm( ofstream &os )
{
	#define PRIMITIVE_CREATE(TYPE) \
	if(strcmp(_type,#TYPE)==0) { \
		Primitive<TYPE> p( (TYPE*) _el, _elNum ); os << p; return; }
	assert( _el!=NULL && _elNum>0 );
	PRIMITIVE_CREATE(int)
	PRIMITIVE_CREATE(long)
	PRIMITIVE_CREATE(float)
	PRIMITIVE_CREATE(double)
	PRIMITIVE_CREATE(bool)
	PRIMITIVE_CREATE(char)
	abortError( "Unknown type", _type, __LINE__, __FILE__ );
}

void			ObjImg::check( int minL, int maxL, const char *name, const char *type )
{
	if( type!=NULL && strcmp(_type,type) )
		abortError( "Invalid type", type, __LINE__, __FILE__ );
	if( name!=NULL && strcmp(_name,name) )
		abortError( "Invalid name:", name, __LINE__, __FILE__ );
	assert(int(_objImgs.size())>=minL );
	assert(int(_objImgs.size())<=maxL );
}

/////////////////////////////////////////////////////////////////////////////////

#define SAVABLE_CREATE(TYPE,OBJ) \
	if (!strcmp(cname, TYPE)) return (Savable*) new OBJ();
#define SAVABLE_CLONE(TYPE,OBJ,SRC) \
	if (!strcmp(cname, TYPE)) return (Savable*)new OBJ(*((OBJ*) SRC));
#define SAVABLE_CLONECOPY(TYPE,OBJ,SRC) \
	if (!strcmp(cname, TYPE)) { OBJ *obj=new OBJ(); (*obj)=*((OBJ*) SRC); return (Savable*) obj; }

Savable*		Savable::createObj( const char* cname ) 
{
	abortError( "unknown type", cname, __LINE__, __FILE__ );
	return NULL;
}

Savable*		Savable::cloneObj( Savable *obj )
{
	const char *cname = obj->getCname();
	abortError( "unknown type", cname, __LINE__, __FILE__ );
	return NULL;
}
