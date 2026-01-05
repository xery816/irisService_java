/*#include <stdio.h>
#include <stdlib.h>
#include <time.h>*/
#if defined(_WIN32) || defined(__CYGWIN__)
#include "windows.h"
#include <direct.h>
#include <conio.h>
#endif

#include <time.h>
#include <sys/time.h> 
#include <unistd.h>
#include <sys/stat.h> 
#include <string.h>
#include <signal.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "base64codec.hpp"

//#include "rwconfig.h"
//#include "misc_utils.h"
//#include "ConfigManager.h"

#include "irisengine_export.h"

using namespace std;
using namespace nlohmann;

#define LOG_TAG 		"console"

///////////////////////////////////////

typedef enum {
	NOTHING,
	ENROLL,
	VERIFY,
	IDENTIFY,
	IDENTIFY2,
	CAPTURE
} IRIS_STATE;

#if defined(_WIN32)
#define PATH_SEPARATOR      '\\'
#else
#define PATH_SEPARATOR      '/'
#endif


#define TEMP_DATA_DIR 	"console-temp"
static string exe_dir;
static bool is_done = false;
static bool inited = false;
static IRIS_STATE state = NOTHING;


static bool file_exists(const char *file_path)
{
	struct stat buf;
	int ret = stat(file_path, &buf);
	return (0==ret);
}
static bool dir_exists(const char *dir_path)
{
	struct stat buf;
	int ret = stat(dir_path, &buf);
	if( ret == 0 && S_ISDIR(buf.st_mode) ) {
		return true;
	}
	return false;
}

int file_size(const char *file_path)
{
	struct stat buf;
	int ret = stat(file_path, &buf);
	if(ret)
		return -1;

	return buf.st_size;	
}

int readFile(const char *file, unsigned char* data, int& len)
{
	if(!file_exists(file))
		return -1;
	len = file_size(file);

	std::ifstream ifs(file, std::ifstream::binary);
	if (ifs) {
		ifs.read((char*)data, len);
		ifs.close();
	}			

	return 0;
}

int writeFile(const char *file, unsigned char* data, int len)
{
	std::ofstream ofs(file, std::ofstream::binary);
	if (ofs) {
		ofs.write((char*)data, len);
		ofs.close();
	}			

	return 0;	
}

static void make_dir(const char *dir_path)
{
#ifdef __linux__
    mkdir(dir_path, S_IRWXU | S_IRWXG);
#else
    _mkdir(dir_path);
#endif    	
}

void saveUser(const char *user_id,
                bool left_exist, unsigned char *left_iris, 
                bool right_exist, unsigned char *right_iris,
                int iris_len)
{
	char user_dir[1024];
	sprintf(user_dir, "%s%s%c%s%c%s", exe_dir.c_str(), TEMP_DATA_DIR, PATH_SEPARATOR, "fetchuser", PATH_SEPARATOR, user_id);            
	make_dir(user_dir);

	if(left_exist) {
		string left_file = std::string(user_dir) + "/left.dat";
		std::ofstream ofs(left_file.c_str(), std::ofstream::binary);
		if (ofs) {
			ofs.write((const char*)left_iris, iris_len);
			ofs.close();
		}		
	}
	if(right_exist) {
		string right_file = std::string(user_dir) + "/right.dat";
		std::ofstream ofs(right_file.c_str(), std::ofstream::binary);
		if (ofs) {
			ofs.write((const char*)right_iris, iris_len);
			ofs.close();
		}		
	}	
}

int readUser(const char *user_id,
                bool *left_exist, unsigned char *left_iris, 
                bool *right_exist, unsigned char *right_iris,
                int iris_len)
{
	char user_dir[1024];
	sprintf(user_dir, "%s%s%c%s%c%s", exe_dir.c_str(), TEMP_DATA_DIR, PATH_SEPARATOR, "adduser", PATH_SEPARATOR, user_id);            
	if(!dir_exists(user_dir))
		return -1;

	{
		*left_exist = false;
		string left_file = std::string(user_dir) + "/left.dat";
		std::ifstream ifs(left_file.c_str(), std::ifstream::binary);
		if (ifs) {
			ifs.read((char*)left_iris, iris_len);
			ifs.close();
			*left_exist = true;
		}		
	}

	{
		*right_exist = false;
		string right_file = std::string(user_dir) + "/right.dat";
		std::ifstream ifs(right_file.c_str(), std::ifstream::binary);
		if (ifs) {
			ifs.read((char*)right_iris, iris_len);
			ifs.close();
			*right_exist = true;
		}		
	}

	return 0;
}

int writePreview(const unsigned char* data, int len)
{
    static int frame_no = 0;
    static int image_no = 0;
    static bool prepared = false;

	char preview_dir[1024];
	sprintf(preview_dir, "%s%s%c%s", exe_dir.c_str(), TEMP_DATA_DIR, PATH_SEPARATOR, "preview");            

	if(!prepared) {
	    make_dir(preview_dir);
	    prepared = true;
	}

	//save image every second(frame rate: ~30)
	if(frame_no++%30 == 0) {
		char preview_file[1024];
	    sprintf(preview_file, "%s/%04d.jpg", preview_dir, image_no++);

		std::ofstream ofs(preview_file, std::ofstream::binary);
		if (ofs) {
			ofs.write((const char*)data, len);

			ofs.close();
			return 0;
		}
		return -1;
	}
	return 0;
}

void decodeEnrollData(const char* user_id, char* a_data) 
{
	unsigned char* temp = (unsigned char*)malloc(1024*1024*4);
    try{
        nlohmann::json user = nlohmann::json::parse(a_data);

        try {
			char user_dir[1024];
			sprintf(user_dir, "%s%s%c%s%c%s", exe_dir.c_str(), TEMP_DATA_DIR, PATH_SEPARATOR, "fetchenroll", PATH_SEPARATOR, user_id);            
			make_dir(user_dir);

            for(auto& item: user["iris"]) {
                std::string which = item["which"];
                {   //image
                    std::string data = item["image"]["data"];
                    std::string format = item["image"]["format"];
                    if(data.size() > 0) {
                        base64codec::Base64Decrypt decoder(data.c_str(), data.size());
                        int size_dec = decoder.GetSize();
                        memcpy(temp, decoder.PlainText(), size_dec);
                        writeFile((std::string(user_dir)+ PATH_SEPARATOR + which + "." + format).c_str(), temp, size_dec);
                    }
                }
                {   //data
                    std::string data = item["data"];
                    if(data.size() > 0) {
                        base64codec::Base64Decrypt decoder(data.c_str(), data.size());
                        int size_dec = decoder.GetSize();
                        memcpy(temp, decoder.PlainText(), size_dec);
                        writeFile((std::string(user_dir)+ PATH_SEPARATOR + which + ".dat").c_str(), temp, size_dec);        
                    }    
                }
                std::cout << which << "\tclarity:" << item["param"]["clarity"]
                		  << " focus:" << item["param"]["focus"]
                		  << " radius:" << item["param"]["radius"]
                		  << " spot:" << item["param"]["spot"]
                		  << " quality:" << item["param"]["quality"]
                		  << std::endl;
            }
        }
        catch (const nlohmann::json::type_error&) {
            std::cerr << "json::type_error, user:" << user_id << std::endl;
        }
        catch (const nlohmann::json::out_of_range&) {
            std::cerr << "json::out_of_range, user:" << user_id << std::endl;
        }
        catch (const std::invalid_argument& ex) {
            std::cerr << "std::invalid_argument error, user:" << user_id << std::endl;
        }
    }
    catch (const nlohmann::json::parse_error&) {
        std::cerr << "json::parse_error, user:" << user_id << std::endl;
    }

    free(temp);
}

void callBack(const char* user_id, iris_callback_result *result)
{
	char msg[1024];

	sprintf(msg, "result:%d, eye:%d, finish:%d", result->result, result->eye, result->finish);
	std::cout << msg << std::endl;
	
	if(result->finish) {
		switch(state) {
		case ENROLL:
			if(IRIS_RESULT(result->result) == IRIS_RESULT_OK) {
				sprintf(msg, "%s enrolled", user_id);

				int len = IrisEng_get_enroll_data_len(user_id);
				char* data = (char*)malloc(len+1);
				if(0 == IrisEng_fetch_enroll_data(user_id, data, len+1)) {
					decodeEnrollData(user_id, data);
				}
    			free(data);
			}
			else if(IRIS_RESULT(result->result) == IRIS_RESULT_FAIL) {
				sprintf(msg, "find user '%s' with same iris trait", user_id);
			}
			else if(IRIS_RESULT(result->result) == IRIS_RESULT_TIMEOUT) {
				sprintf(msg, "enroll timeout");
			}
			state = NOTHING;
			break;

		case IDENTIFY:
		case IDENTIFY2:
			if(IRIS_RESULT(result->result) == IRIS_RESULT_OK) {
				sprintf(msg, "%s identified", user_id);
			}
			else if(IRIS_RESULT(result->result) == IRIS_RESULT_FAIL) {
				sprintf(msg, "no user identified");
			}
			else if(IRIS_RESULT(result->result) == IRIS_RESULT_FAIL_LR_MATCH) {
				sprintf(msg, "find different user with same left/right iris");
			}
			else if(IRIS_RESULT(result->result) == IRIS_RESULT_TIMEOUT) {
				sprintf(msg, "identify timeout");
			}
			if(state == IDENTIFY) {
				state = NOTHING;
			}
			break;

		default:
			break;
		}
		std::cout << msg << std::endl;
	}
}

void previewCB(const unsigned char* data, int len, int width, int height, int flag)
{
	writePreview(data, len);
}

void soundCB(const int sound_id)
{
	static int alert_count = 0;

	if(sound_id != IRIS_SOUND_ALERT) {
		std::cout << "receive iris sound:" << sound_id << std::endl;
	}
	else {
		if(alert_count++ == 20) {
			std::cout << "receive iris sound:" << sound_id << std::endl;
			alert_count = 0;
		}
	}
}

void preaction()
{
	std::string cmd = exe_dir + "preaction.sh";
	if(file_exists(cmd.c_str())) {
		system(cmd.c_str());
		std::cout << "exec: " << cmd << std::endl;
	}
}


#if defined(_WIN32)
static HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCTSTR)GetCurrentModule,
			&hModule
	);

  return hModule;
}
#endif

int get_self_dir()
{
	int ret;

	char buf[1024];
	int buf_len = sizeof(buf);

#if defined(_WIN32) || defined(__CYGWIN__)
	//ret = GetModuleFileNameA( NULL, buf, 256 );
	HMODULE hm = GetCurrentModule();
	ret = GetModuleFileNameA(hm, buf, buf_len);
	if (0 == ret)    goto FAIL_EXIT;
#else
    // TODO
	ret = readlink("/proc/self/exe", buf, buf_len);
	if (-1 == ret)   goto FAIL_EXIT;
#endif

	exe_dir = buf;
	exe_dir = exe_dir.substr(0, exe_dir.find_last_of("/\\")+1);
	std::cout << "exe directory: " << exe_dir << std::endl;
	return 0;	

FAIL_EXIT:
	return -1;	
}

int readConfigFile(const char *path, char *pbuf, int len)
{
    ifstream ifs(path);
    if(!ifs.is_open()) {
        return -1;
    }
    string tmp, str;
    while(getline(ifs, tmp)) {
        if(tmp.size()) {
            size_t found = tmp.find_first_not_of(" \t");
            if(found != string::npos && tmp[found] != '#') {
                str += tmp;
            }
        }
    }
    ifs.close();
    strncpy(pbuf, str.c_str(), len);
    return 0;
}

void eventCallBack(const IrisEventInfo *event_info)
{
	switch(event_info->event) {
	case IRIS_EVENT_DEV_PLUG:
		std::cout << "detect device plugged in, need to init manually" << std::endl;
		//init();
		break;
	case IRIS_EVENT_DEV_UNPLUG:
		std::cout << "detect device unplugged, need to release manually" << std::endl;
		//IrisEng_release();
		break;		
	default:
		break;
	}
}

void init()
{
	char conf_buf[4096];
	string cfg_file = "param_common.cfg";
	string my_cfg = exe_dir + cfg_file;
	if(readConfigFile(my_cfg.c_str(), conf_buf, 4095)) {
		cerr << "readConfigFile fail: " << cfg_file << endl;
		exit(1);
	}

	int ret = IrisEng_initiate(conf_buf, eventCallBack);
	if(ret) {
		cerr << "irisEng_initiate fail, err:" << ret << endl;
	}
	else {
		inited = true;
		std::cout << "iris engine initialized" << std::endl;

		cfg_file = "param_dev.cfg";
		my_cfg = exe_dir + cfg_file;
		if(readConfigFile(my_cfg.c_str(), conf_buf, 4095)) {
			cerr << "readConfigFile fail: " << cfg_file << endl;
			exit(1);
		}

		ret = IrisEng_load_dev_params(conf_buf);
	}	

	char ver[32];
	char date[32];
	IrisEng_get_sdk_version(ver, date);
	std::cout << "SDK version: " << ver << std::endl;

	IrisEng_get_algor_version(ver, date);
	std::cout << "Algor version: " << ver << std::endl;
}
///////////////////////////////////////

#ifdef _WIN32
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
		case CTRL_C_EVENT:
			//MessageBox(NULL,"CTRL+C received!","CEvent",MB_OK);
			break;
		case CTRL_BREAK_EVENT:
			//MessageBox(NULL,"CTRL+BREAK received!","CEvent",MB_OK);
			break;
		case CTRL_CLOSE_EVENT:
			//MessageBox(NULL,"Program being closed!","CEvent",MB_OK);
			break;
		case CTRL_LOGOFF_EVENT:
			//MessageBox(NULL,"User is logging off!","CEvent",MB_OK);
			break;
		case CTRL_SHUTDOWN_EVENT:
			//MessageBox(NULL,"User is logging off!","CEvent",MB_OK);
			break;
	}

	IrisEng_release();
	exit(0);

	return TRUE;
}
#endif


#ifdef __linux__    
void sig_handler(int sig)
{
    printf("signal catch:%d\n", sig);
}
#endif


int main()
{
    std::string cmd;
    int ret;

#ifdef __linux__    
    signal(SIGABRT, sig_handler);
#endif

    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    printf("Current local time and date: %s    ", asctime(timeinfo));

	get_self_dir();

	init();


	char dir_1[1024];
	char dir_2[1024];
	sprintf(dir_1, "%s%s", exe_dir.c_str(), TEMP_DATA_DIR);     
	make_dir(dir_1);

	sprintf(dir_2, "%s%c%s", dir_1, PATH_SEPARATOR, "preview");   
	make_dir(dir_2); 
	sprintf(dir_2, "%s%c%s", dir_1, PATH_SEPARATOR, "adduser");   
	make_dir(dir_2); 
	sprintf(dir_2, "%s%c%s", dir_1, PATH_SEPARATOR, "fetchuser");   
	make_dir(dir_2); 
	sprintf(dir_2, "%s%c%s", dir_1, PATH_SEPARATOR, "fetchenroll");   
	make_dir(dir_2); 	

    std::cout << ": " << std::flush;

#ifdef _WIN32
	std::cin.sync_with_stdio(false);

	if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == 0) {
		printf("SetConsoleCtrlHandler fail");
		return -1;
	}
#endif

    while (true)
    {
#ifdef _WIN32
    	MSG msg;
    	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				std::cout << "receive WM_QUIT" << std::endl;
			    break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			std::cout << ": " << std::flush;
		}
	    if (!kbhit()) {
	    	Sleep(100);
	    	continue;
	    }
#endif

	    //char buffer[1024];
	    //cin.getline(buffer, sizeof(buffer));
    	std::getline(std::cin, cmd);

		std::stringstream ss(cmd);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, ' ')) {
			elems.push_back(item);
		}    	
		if(elems.size() == 0)
			continue;

		if(elems[0] == "q" || elems[0] == "quit") {
			break;
		}
		if(elems[0] == "h" || elems[0] == "help") {
			std::cout << "command:" << 
				"\n\tinit                     initialize engine lib" << 
				"\n\trelease                  release engine lib" << 
				"\n\tenroll  <userid>         start enrolling of <userid>" <<
				"\n\tidentify  <-s>           start user identifying" <<
				"\n\t    -s indicates single mode, continuous mode otherwise" << 
				"\n\tstop                     stop enroll/identify" <<
				"\n\tuserdir                  get user data directory" << 
				"\n\tuserlist                 get user list" << 
				"\n\tdeluser  <userid>        delete <userid>" << 
				"\n\tdelalluser               delete all users" << 
				"\n\tfetchuser  <userid>      fetch <userid>" << 
				"\n\tadduser  <userid>        add <userid>" << 
				"\n\tdevinfo                  display device info" << 
				"\n\tversion                  display algor/sdk version" << 
				"\n\tpreview   <on/off>       turn on/off preview" << 
				"\n\tsoundCB   <on/off>       turn on/off sound callback" << 
				std::endl;

		}
		else if(elems[0] == "init") {
			init();
		}		
		else if(elems[0] == "enroll") {
			std::string user = "testuser";
			if(elems.size() >= 2)
				user = elems[1];
			preaction();
			if(ret = IrisEng_enroll(user.c_str(), 1, callBack)) {
				cerr << "IrisEng_enroll fail, err:" << ret << endl;
			}
			else {
				state = ENROLL;
			}
		}
		else if(elems[0] == "identify") {
			bool continuous = true;
			if(elems.size() >= 2) {
				if(elems[1] == "-s" || elems[1] == "-S")
					continuous = false;
			}
			preaction();
			if(ret = IrisEng_identify(NULL, continuous, callBack)) {
				cerr << "irisEng_identify fail, err:" << ret << endl;
			}
			else {
				state = continuous ? IDENTIFY2 : IDENTIFY;
			}			
		}	
		else if(elems[0] == "userdir") {
			char user_dir[1024];
			if(0 == IrisEng_get_user_dir(user_dir)) {
				std::cout << "user dir: " << user_dir << std::endl;
			}
		}		
		else if(elems[0] == "userlist") {
		    char* l_user_list = NULL;
		    int l_user_num;

		    if(0 == IrisEng_get_user_list(&l_user_num, NULL)) {
			    if(l_user_num) {
				    l_user_list = (char*)calloc(l_user_num, IRIS_USER_ID_LENGTH);
				    IrisEng_get_user_list(&l_user_num, l_user_list);
				    std::cout << "user list: " << l_user_list << std::endl;
				}
			}
		}		
		else if(elems[0] == "deluser") {
			if(elems.size() >= 2) {
				IrisEng_delete_user(elems[1].c_str(), true);			
				std::cout << "user '" << elems[1] << "' deleted'" << std::endl;
			}
		}		
		else if(elems[0] == "delalluser") {
			IrisEng_delete_all_user(true);		
			std::cout << "all users deleted" << std::endl;	
		}				
		else if(elems[0] == "fetchuser") {
			if(elems.size() >= 2) {
				int len = IrisEng_get_user_code_len();
				bool left_exist = false;
                bool right_exist = false;
				unsigned char *left_iris = (unsigned char*)calloc(1, len);
				unsigned char *right_iris = (unsigned char*)calloc(1, len);
				int ret = IrisEng_fetch_user(elems[1].c_str(), &left_exist, left_iris, &right_exist, right_iris);	
				if(ret) {
					std::cout << "user " << elems[1] << " not found" << std::endl;	
				}
				else {
					std::cout << "user " << elems[1] << " saved, " << "left: " << left_exist << ", right: " << right_exist << std::endl;	
					saveUser(elems[1].c_str(), left_exist, left_iris, right_exist, right_iris, len);
				}
			}
		}				
		else if(elems[0] == "adduser") {
			if(elems.size() >= 2) {
				int len = IrisEng_get_user_code_len();
				bool left_exist = false;
                bool right_exist = false;
				unsigned char *left_iris = (unsigned char*)calloc(1, len);
				unsigned char *right_iris = (unsigned char*)calloc(1, len);
				if(readUser(elems[1].c_str(), &left_exist, left_iris, &right_exist, right_iris, len)) {
					std::cout << "user " << elems[1] << " not found, add user fail" << std::endl;
				}
				else {
					unsigned char *left_ptr  = left_exist  ? left_iris  : NULL;
					unsigned char *right_ptr = right_exist ? right_iris : NULL;
					if(ret = IrisEng_add_user(elems[1].c_str(), left_ptr, right_ptr, 1)) {
						std::cout << "add user " << elems[1] << " fail, error: " << ret << std::endl;
					}
					else {
						std::cout << "add user " << elems[1] << " OK" << std::endl;
					}
				}
			}
		}						
		else if (elems[0] == "devinfo") {
			IrisDeviceInfo dev_info;
			IrisEng_get_device_info(&dev_info);
			std::cout << "firmware ver:    " << dev_info.fw_ver << std::endl;
			std::cout << "firmware date:   " << dev_info.fw_date << std::endl;
			std::cout << "manufacturer:    " << dev_info.manufacturer << std::endl;
			std::cout << "LOGO:            " << dev_info.logo << std::endl;
			std::cout << "SN:              " << dev_info.sn << std::endl;
			std::cout << "max user number: " << dev_info.max_user_num << std::endl;
		}
		else if (elems[0] == "version") {
			char version[32], date[32];
			IrisEng_get_algor_version(version, date);
			std::cout << "algor --- ver: " << version << "  date: " << date << std::endl;
			IrisEng_get_sdk_version(version, date);
			std::cout << "sdk   --- ver: " << version << "  date: " << date << std::endl;
		}
		else if(elems[0] == "preview") {
			if(elems.size() >= 2) {
				if(elems[1] == "on") {
					IrisEng_set_preview("whole", IMAGE_PIXEL_RAW, 0, 0, previewCB);
					std::cout << "preview turned on" << std::endl;	
				}
				else{
					IrisEng_set_preview("whole", IMAGE_PIXEL_RAW, 0, 0, NULL);
					std::cout << "preview turned off" << std::endl;	
				}
			}
		}						
		else if(elems[0] == "soundCB") {
			if(elems.size() >= 2) {
				if(elems[1] == "on") {
					IrisEng_set_sound_player(soundCB);
					std::cout << "sound callback turned on" << std::endl;	
				}
				else{
					IrisEng_set_sound_player(NULL);
					std::cout << "sound callback turned off" << std::endl;	
				}
			}
		}								
		else if(elems[0] == "stop") {
			IrisEng_stop();
			std::cout << "action stopped" << std::endl;	
		}
		else if(elems[0] == "release") {
			IrisEng_release();
			inited = false;
			std::cout << "iris engine released!" << std::endl;
		}
		else {
			cout << "command '" << elems[0] << "' is not supported" << endl;
		}

		std::cout << ": " << std::flush;
    }

    if(inited) {
    	IrisEng_release();
    }
	
	return 0;
}
