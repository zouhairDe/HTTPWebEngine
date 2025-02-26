
# TODO:

## zouhairDe
	0. Firsti should make the indeFile -> IndexFiles: vector<string>, and its not necessary in the config file
	
	/*		
		else if (fileFromUri.empty()) {
			cerr << red << "File from uri is empty, Meanning we r in a Folder\n" << def;
			//TODO: check if the folder exists and we have access to it
				/* If yes check if indexFiles of this route are valid starting from the left side*/
					/* If a file is valid return it*/
				/* Else check if directory listing is On */
					/* If yes generate httpFile for DL */
					/* Else return nullptr: for 404errpage */

			return nullptr;
		}
	*/
	
	1. In: File *Route::getGETResponse(RequestProccessor req, string root, string fileFromUri) const
		Ihave to check if it a folder or a file to responde 3la hsab ila folder wach directory listenning On wlala

## AccioMo
	1. understand cgi
	2. implement it
	3. fail
	4. ...?
	5. profit

## later
	1. handle request and response
