#include "sql3DataAccess.h"
#include <vector>


int album_id;
int picture_id;
int user_id;
int tempCount;
std::list<Album> albums;
std::list<User> users;

std::list<Picture> pictures;
sqlite3* db;
int getAlbumId(void* data, int argc, char** argv, char** azColName);
int addAlbum(void* data, int argc, char** argv, char** azColName);
int getUserId(void* data, int argc, char** argv, char** azColName);
int getPictureId(void* data, int argc, char** argv, char** azColName);
int addUser(void* data, int argc, char** argv, char** azColName);
int count(void* data, int argc, char** argv, char** azColName);
int addPicture(void* data, int argc, char** argv, char** azColName);
int addUserId(void* data, int argc, char** argv, char** azColName);
// album related
const std::list<Album> sql3DataAccess::getAlbums() {
	std::string sqlStatement = "SELECT * FROM ALBUMS;";
	char* errMessage = nullptr;
	albums = *(new std::list<Album>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addAlbum, nullptr, &errMessage);
	return albums;
}
int addAlbum(void* data, int argc, char** argv, char** azColName)
{
	Album a = *(new Album());
	int id;
	for (int i = 0; i < argc; i++)
	{
		std::string colName(azColName[i]);
		if (colName == "NAME") {
			a.setName(argv[i]);
		}
		else if (colName == "USER_ID") {
			a.setOwner(atoi(argv[i]));
		}
		else if (colName == "CREATION_DATE") {
			a.setCreationDate(argv[i]);
		}
		else if (colName == "ID") {
			id = atoi(argv[i]);
		}
	}
	std::string sqlStatement = "SELECT * FROM PICTURES WHERE ALBUM_ID = " + std::to_string(id) + "; ";
	char* errMessage = nullptr;
	pictures = *(new std::list<Picture>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addPicture, nullptr, &errMessage);
	std::vector<Picture> myVector(pictures.begin(), pictures.end());
	for (int i = 0; i < myVector.size(); i++) {
		a.addPicture(myVector[i]);
	}
	albums.push_back(a);
	return 0;
}
const std::list<Album> sql3DataAccess::getAlbumsOfUser(const User& user) {
	std::string sqlStatement = "SELECT * FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	char* errMessage = nullptr;
	albums = *(new std::list<Album>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addAlbum, nullptr, &errMessage);
	return albums;
}
void sql3DataAccess::createAlbum(const Album& album) {
	std::string sqlStatement = "INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('" + album.getName() + "', " + std::to_string(album.getOwnerId()) + ", '" + album.getCreationDate() +"');";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
void sql3DataAccess::deleteAlbum(const std::string& albumName, int userId) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = '" + std::to_string(userId) +"';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST; 
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);
	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID IN (SELECT ID FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + ");DELETE FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + ";DELETE FROM ALBUMS WHERE ID = " + std::to_string(album_id) + ";";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}

bool sql3DataAccess::doesAlbumExists(const std::string& albumName, int userId) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE USER_ID = '" + std::to_string(userId) + "' AND NAME = '" + albumName +"';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST;
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);

	return album_id != ID_NOT_EXIST;
}
Album sql3DataAccess::openAlbum(const std::string& albumName) {
	std::string sqlStatement = "SELECT * FROM ALBUMS WHERE NAME = '" + albumName + "';";
	char* errMessage = nullptr;
	albums = *(new std::list<Album>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addAlbum, nullptr, &errMessage);
	std::vector<Album> myVector(albums.begin(), albums.end());
	return myVector[0];
}
void sql3DataAccess::closeAlbum(Album& pAlbum) {

}
void sql3DataAccess::printAlbums() {
	std::string sqlStatement = "SELECT * FROM ALBUMS;";
	char* errMessage = nullptr;
	albums = *(new std::list<Album>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addAlbum, nullptr, &errMessage);
	std::vector<Album> myVector(albums.begin(), albums.end());
	for (int i = 0; i < myVector.size(); i++) {
		std::cout << myVector[i];
	}
}
int getAlbumId(void* data, int argc, char** argv, char** azColName)
{
	album_id = atoi(argv[0]);
	return 0;
}

// picture related
void sql3DataAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName +"';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST; 
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);
	sqlStatement = "INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) VALUES ('" + picture.getName() + "', '" + picture.getPath() + "', '" + picture.getCreationDate() + "', " + std::to_string(album_id)+ ");";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}

void sql3DataAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST; 
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);
	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID IN (SELECT ID FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + " AND NAME = '" + pictureName + "');DELETE FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) +" AND NAME = '" + pictureName +"';";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
void sql3DataAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST; 
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);
	sqlStatement = "SELECT ID FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + " AND NAME = '" + pictureName + "';";
	errMessage = nullptr;
	picture_id = ID_NOT_EXIST; 
	res = sqlite3_exec(db, sqlStatement.c_str(), getPictureId, nullptr, &errMessage);
	sqlStatement = "INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (" + std::to_string(picture_id) + ", " + std::to_string(userId) + ");";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
void sql3DataAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {
	std::string sqlStatement = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "';";
	char* errMessage = nullptr;
	album_id = ID_NOT_EXIST; 
	int res = sqlite3_exec(db, sqlStatement.c_str(), getAlbumId, nullptr, &errMessage);
	sqlStatement = "SELECT ID FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + " AND NAME = '" + pictureName + "';";
	errMessage = nullptr;
	picture_id = ID_NOT_EXIST; 
	res = sqlite3_exec(db, sqlStatement.c_str(), getPictureId, nullptr, &errMessage);
	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture_id) + " AND USER_ID = " + std::to_string(userId) + ";";
	res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
int getPictureId(void* data, int argc, char** argv, char** azColName) {
	picture_id = atoi(argv[0]);
	return 0;
}

// user related
int addUser(void* data, int argc, char** argv, char** azColName)
{
	std::string name;
	int id;
	for (int i = 0; i < argc; i++)
	{
		std::string colName(azColName[i]);
		if (colName == "NAME") {
			name = argv[i];
		}
		else if (colName == "ID") {
			id = atoi(argv[i]);
		}

	}
	User u = *(new User(id,name));
	users.push_back(u);
	return 0;
}
void sql3DataAccess::printUsers() {
	std::string sqlStatement = "SELECT * FROM USERS;";
	char* errMessage = nullptr;
	users = *(new std::list<User>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addUser, nullptr, &errMessage);
	std::vector<User> myVector(users.begin(), users.end());
	for (int i = 0; i < myVector.size(); i++) {
		std::cout << myVector[i];
	}
}
User sql3DataAccess::getUser(int userId) {
	std::string sqlStatement = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId) + ";";
	char* errMessage = nullptr;
	users = *(new std::list<User>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addUser, nullptr, &errMessage);
	std::vector<User> myVector(users.begin(), users.end());
	return myVector[0];
}
void sql3DataAccess::createUser(User& user) {
	std::string sqlStatement = "INSERT INTO USERS (ID, NAME) VALUES (" + std::to_string(user.getId()) +", '" + user.getName()+ "');";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
void sql3DataAccess::deleteUser(const User& user) {
	std::string sqlStatement = "DELETE FROM USERS WHERE ID = " + std::to_string(user.getId()) + " AND NAME = '" + +"';" + ", '" + user.getName() + "');";
	int res = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, nullptr);
}
bool sql3DataAccess::doesUserExists(int userId) {
	std::string sqlStatement = "SELECT ID FROM USERS WHERE ID = " + std::to_string(userId) + ";";
	char* errMessage = nullptr;
	user_id = ID_NOT_EXIST;
	int res = sqlite3_exec(db, sqlStatement.c_str(), getUserId, nullptr, &errMessage);

	return user_id != ID_NOT_EXIST;
}
int getUserId(void* data, int argc, char** argv, char** azColName) {
	user_id = atoi(argv[0]);
	return 0;
}

// user statistics
int sql3DataAccess::countAlbumsOwnedOfUser(const User& user) {
	return getAlbumsOfUser(user).size();
}
int sql3DataAccess::countAlbumsTaggedOfUser(const User& user) {
	std::string sqlStatement = "SELECT DISTINCT ALBUM_ID FROM PICTURES WHERE ID IN (SELECT PICTURE_ID FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ");";
	char* errMessage = nullptr;
	tempCount = 0;
	int res = sqlite3_exec(db, sqlStatement.c_str(), count, nullptr, &errMessage);
	return tempCount;

}
int sql3DataAccess::countTagsOfUser(const User& user) {
	std::string sqlStatement = "SELECT * FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	char* errMessage = nullptr;
	tempCount = 0;
	int res = sqlite3_exec(db, sqlStatement.c_str(), count, nullptr, &errMessage);
	return tempCount;
}
int sql3DataAccess::countTagsOfPicture(const Picture& picture) {
	std::string sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture.getId()) + ";";
	char* errMessage = nullptr;
	tempCount = 0;
	int res = sqlite3_exec(db, sqlStatement.c_str(), count, nullptr, &errMessage);
	return tempCount;
}
float sql3DataAccess::averageTagsPerAlbumOfUser(const User& user) {
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}
int count(void* data, int argc, char** argv, char** azColName)
{
	tempCount++;
	return 0;
}
// queries
User sql3DataAccess::getTopTaggedUser() {
	std::string sqlStatement = "SELECT * FROM USERS;";
	char* errMessage = nullptr;
	users = *(new std::list<User>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addUser, nullptr, &errMessage);
	std::vector<User> myVector(users.begin(), users.end());
	int max = -1;
	User maxUser = myVector[0];
	for (int i = 0; i < myVector.size(); i++) {
		if (countTagsOfUser(myVector[i]) > max) {
			max = countTagsOfUser(myVector[i]);
			maxUser = myVector[i];
		}
	}
	return maxUser;
}
Picture sql3DataAccess::getTopTaggedPicture() {
	std::string sqlStatement = "SELECT * FROM PICTURES;";
	char* errMessage = nullptr;
	pictures = *(new std::list<Picture>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addPicture, nullptr, &errMessage);
	std::vector<Picture> myVector(pictures.begin(), pictures.end());
	int max = -1;
	Picture maxPic = myVector[0];
	for (int i = 0; i < myVector.size(); i++) {
		if (countTagsOfPicture(myVector[i]) > max) {
			max = countTagsOfPicture(myVector[i]);
			maxPic = myVector[i];
		}
	}
	return maxPic;
}
std::list<Picture> sql3DataAccess::getTaggedPicturesOfUser(const User& user) {
	std::string sqlStatement = "SELECT DISTINCT * FROM PICTURES WHERE ID IN(SELECT PICTURE_ID FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + "); ";
	char* errMessage = nullptr;
	pictures = *(new std::list<Picture>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addPicture, nullptr, &errMessage);
	return pictures;
}
int addPicture(void* data, int argc, char** argv, char** azColName)
{
	int id;
	std::string name;
	std::string path;
	std::string creationDate;
	int albumId;
	for (int i = 0; i < argc; i++)
	{
		std::string colName(azColName[i]);
		if (colName == "NAME") {
			name = argv[i];
		}
		else if (colName == "ID") {
			id = atoi(argv[i]);
		}
		else if (colName == "LOCATION") {
			path = argv[i];
		}
		else if (colName == "CREATION_DATE") {
			creationDate = argv[i];
		}
		else if (colName == "ALBUM_ID") {
			albumId = atoi(argv[i]);
		}

	}
	std::string sqlStatement = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID = " + std::to_string(id) + "; ";
	char* errMessage = nullptr;
	users = *(new std::list<User>());
	int res = sqlite3_exec(db, sqlStatement.c_str(), addUserId, nullptr, &errMessage);
	Picture p = *(new Picture(id,name,path,creationDate));
	std::vector<User> myVector(users.begin(), users.end());
	for (int i = 0; i < myVector.size(); i++) {
		p.tagUser(myVector[i]);
	}
	pictures.push_back(p);
	return 0;
}

int addUserId(void* data, int argc, char** argv, char** azColName)
{
	std::string sqlStatement = "SELECT * FROM USERS WHERE ID = " + std::to_string(atoi(argv[0])) + ";";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement.c_str(), addUser, nullptr, &errMessage);
	return 0;
}
bool sql3DataAccess::open() {
	std::string dbFileName = "galleryDB.sqlite";
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}
	return true;
}
void sql3DataAccess::close() {
	sqlite3_close(db);
	db = nullptr;
}
void sql3DataAccess::clear() {

}

