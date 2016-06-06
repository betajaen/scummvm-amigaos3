/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef BACKENDS_CLOUD_GOOGLEDRIVE_GOOGLEDRIVESTORAGE_H
#define BACKENDS_CLOUD_GOOGLEDRIVE_GOOGLEDRIVESTORAGE_H

#include "backends/cloud/storage.h"
#include "common/callback.h"
#include "backends/networking/curl/curljsonrequest.h"

namespace Cloud {
namespace GoogleDrive {

class GoogleDriveStorage: public Cloud::Storage {
	static char *KEY, *SECRET;

	static void loadKeyAndSecret();

	Common::String _token, _refreshToken;

	/** This private constructor is called from loadFromConfig(). */
	GoogleDriveStorage(Common::String token, Common::String refreshToken);

	/**
	 * This private constructor is called from authThroughConsole() (phase 2).
	 * It uses OAuth code flow to get tokens.
	 */
	GoogleDriveStorage(Common::String code);

	void tokenRefreshed(BoolCallback callback, Networking::JsonResponse response);
	void codeFlowComplete(BoolResponse response);

	/** Constructs StorageInfo based on JSON response from cloud. */
	void infoInnerCallback(StorageInfoCallback outerCallback, Networking::JsonResponse json);

	/** Returns bool based on JSON response from cloud. */
	void createDirectoryInnerCallback(BoolCallback outerCallback, Networking::JsonResponse json);

	void printJson(Networking::JsonResponse response);
	void fileDownloaded(BoolResponse response);
	void printFiles(FileArrayResponse response);
	void printBool(BoolResponse response);
	void printFile(UploadResponse response);
	void printInfo(StorageInfoResponse response);

	void fileInfoCallback(Networking::NetworkReadStreamCallback outerCallback, Networking::JsonResponse response);	
public:	
	virtual ~GoogleDriveStorage();

	/**
	 * Storage methods, which are used by CloudManager to save
	 * storage in configuration file.
	 */

	/**
	 * Save storage data using ConfMan.
	 * @param keyPrefix all saved keys must start with this prefix.
	 * @note every Storage must write keyPrefix + "type" key
	 *       with common value (e.g. "Dropbox").
	 */
	virtual void saveConfig(Common::String keyPrefix);

	/** Public Cloud API comes down there. */

	/** Returns StorageFile with the resolved file's id. */
	virtual Networking::Request *resolveFileId(Common::String path, UploadCallback callback, Networking::ErrorCallback errorCallback);

	/** Returns Array<StorageFile> - the list of files. */
	virtual Networking::Request *listDirectory(Common::String path, ListDirectoryCallback callback, Networking::ErrorCallback errorCallback, bool recursive = false);

	/** Returns Array<StorageFile> - the list of files. */
	virtual Networking::Request *listDirectoryById(Common::String id, ListDirectoryCallback callback, Networking::ErrorCallback errorCallback);

	/** Returns UploadStatus struct with info about uploaded file. */
	virtual Networking::Request *upload(Common::String path, Common::SeekableReadStream *contents, UploadCallback callback, Networking::ErrorCallback errorCallback);

	/** Returns pointer to Networking::NetworkReadStream. */
	virtual Networking::Request *streamFile(Common::String path, Networking::NetworkReadStreamCallback callback, Networking::ErrorCallback errorCallback);

	/** Calls the callback when finished. */
	virtual Networking::Request *remove(Common::String path, BoolCallback callback, Networking::ErrorCallback errorCallback) { return nullptr; } //TODO

	/** Calls the callback when finished. */
	virtual Networking::Request *createDirectory(Common::String path, BoolCallback callback, Networking::ErrorCallback errorCallback);

	/** Calls the callback when finished. */
	virtual Networking::Request *createDirectoryWithParentId(Common::String parentId, Common::String name, BoolCallback callback, Networking::ErrorCallback errorCallback);

	/** Returns the StorageInfo struct. */
	virtual Networking::Request *info(StorageInfoCallback callback, Networking::ErrorCallback errorCallback);

	/** Returns storage's saves directory path with the trailing slash. */
	virtual Common::String savesDirectoryPath();

	/**
	 * Load token and user id from configs and return GoogleDriveStorage for those.	
	 * @return pointer to the newly created GoogleDriveStorage or 0 if some problem occured.
	 */
	static GoogleDriveStorage *loadFromConfig(Common::String keyPrefix);

	/**
	 * Returns GoogleDrive auth link.
	 */
	static Common::String getAuthLink();

	/**
	 * Show message with GoogleDrive auth instructions. (Temporary)
	 */
	static void authThroughConsole();

	/**
	 * Gets new access_token. If <code> passed is "", refresh_token is used.
	 * Use "" in order to refresh token and pass a callback, so you could
	 * continue your work when new token is available.
	 */
	void getAccessToken(BoolCallback callback, Common::String code = "");

	Common::String accessToken() { return _token; }
};

} // End of namespace GoogleDrive
} // End of namespace Cloud

#endif
