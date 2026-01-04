#ifndef __IRISENGINE_EXPORT_H__
#define __IRISENGINE_EXPORT_H__


#define IRIS_ENG_OK                         0
#define IRIS_ENG_ERR_NO_DEV                 1
#define IRIS_ENG_ERR_DEV_CONNECT            2
#define IRIS_ENG_ERR_DEV_ILLEGAL            3
#define IRIS_ENG_ERR_DEV_CAPTURE            4
#define IRIS_ENG_ERR_DEV_HOTPLUG_ENABLE     6
#define IRIS_ENG_ERR_DEV_NOT_INIT           7
#define IRIS_ENG_ERR_DEV_ALREADY_INIT       8
#define IRIS_ENG_ERR_DEV_CONFIG             9
#define IRIS_ENG_ERR_DEV_LOG                10
#define IRIS_ENG_ERR_DEV_FRAME_PARAM        11  
#define IRIS_ENG_ERR_DEV_EU_CTRL            12
#define IRIS_ENG_ERR_DEV_EXPIRE             15
#define IRIS_ENG_ERR_DEV_INFO               16
#define IRIS_ENG_ERR_DEV_UNAUTHORIZED       17

#define IRIS_ENG_ERR_NOT_INIT               20
#define IRIS_ENG_ERR_ALREADY_INIT           21
#define IRIS_ENG_ERR_WRONG_CONFIG           22
#define IRIS_ENG_ERR_MAX_USER_NUM           23
#define IRIS_ENG_ERR_USER_ID_ILLEGAL        24
#define IRIS_ENG_ERR_USER_EXIST             25
#define IRIS_ENG_ERR_NO_USER                26
#define IRIS_ENG_ERR_ALREADY_RUN            27
#define IRIS_ENG_ERR_USER_NOT_EXIST         28
#define IRIS_ENG_ERR_SOUND_PLAY             30
#define IRIS_ENG_ERR_MISSING_CONFIG         31
#define IRIS_ENG_ERR_UNALLOWED_REGION       32
#define IRIS_ENG_ERR_NO_CIQ_LIBRARY         33
#define IRIS_ENG_ERR_SYSTEM                 50


#define IRIS_EXTRACT_OK                     0
#define IRIS_EXTRACT_NOT_IMAGE              -1
#define IRIS_EXTRACT_CREATE_FAIL            -2
#define IRIS_EXTRACT_RADIUS_SMALL           -3
#define IRIS_EXTRACT_RADIUS_BIG             -4
#define IRIS_EXTRACT_MOVE_UP                -5
#define IRIS_EXTRACT_MOVE_DOWN              -6
#define IRIS_EXTRACT_MOVE_RIGHT             -7
#define IRIS_EXTRACT_MOVE_LEFT              -8
#define IRIS_EXTRACT_NOT_IRIS               -9
#define IRIS_EXTRACT_BAD_QUALITY            -10
#define IRIS_EXTRACT_FEW_EXPOSURE           -11



#define IRIS_USER_ID_LENGTH		64

/* enroll/identify/verify result */
typedef enum {
	IRIS_RESULT_OK = 0,
	IRIS_RESULT_TIMEOUT = 1,
	IRIS_RESULT_FAIL = 2,
	IRIS_RESULT_FAIL_LR_MATCH = 3,
    IRIS_RESULT_STRANGER = 4,
    IRIS_RESULT_NOT_REAL = 5,
} IRIS_RESULT;

/* iris event */
typedef enum {
	IRIS_EVENT_DEV_UNPLUG = 1,     //iris device is unplugged
	IRIS_EVENT_DEV_PLUG = 2,       //iris device is plugged
} IRIS_EVENT;


typedef enum {
    IMAGE_PIXEL_RGB565 = 0,
    IMAGE_PIXEL_RGB888 = 1,
    IMAGE_PIXEL_ARGB8888 = 2,
    IMAGE_PIXEL_RAW = 3,
}IMAGE_PIXEL_FORMAT;

/**
 * Description: iris result used by callback function
 */
typedef struct {
    unsigned char result; /* value in enum IRIS_RESULT */
    unsigned char eye;    /* 0: left eye, 1: right eye */
    unsigned char finish; /* 0: unfinished, 1: finish */
} iris_callback_result;

typedef struct {
    unsigned char event; 
    unsigned int param; 
} IrisEventInfo;
#define IrisEventDefined

typedef struct {
    char fw_ver[32];
    char fw_date[32];
    
    char manufacturer[32];
    char logo[32];
    char sn[32];

    unsigned int max_user_num;
} IrisDeviceInfo;

typedef enum {
    IRIS_FILTER_ALL = 0,
    IRIS_FILTER_VALID = 1,
}IRIS_FILTER_MODE;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Description: The iris result callback function provided by App to IrisEngine.
 * Parameter:
 *      @user_id:   the user who is enrolled, identified
 *      @result:    the result of enroll/verify
 */
typedef void (* CallBack)(const char* user_id, iris_callback_result *result);


/**
 * Description: The event callback function provided by App to IrisEngine.
 * Parameter:
 *      @event_info:   event info
 */
typedef void (* IrisEventCB)(const IrisEventInfo *event_info);

/**
 * Description: The preview callback function provided by App to IrisEngine.
 * Parameter:
 *      @data:      image data
 *      @len:       image data size
 *      @width:     image width
 *      @height:    image height
 *      @flag:      -1: whole, 0: left, 1: right
 */
typedef void (* PreviewCB)(const unsigned char* data, int len, int width, int height, int flag);

/**
 * Description: Real-time iris data callback
 * Parameter:
 *      @eye:       0: left, 1: right
 *      @image:     pointer to image gray data
 *      @size:      size of gray data
 *      @width:     image width
 *      @height:    image height
 *      @param:     pointer to iris param in json format
 */
typedef void(* RTIrisCB)(int eye, unsigned char* image, int size, int width, int height, void* param);


/**
 * Description: Initialize the iris engine.
 * Parameter:
 *      @configuration:  The configuration in json format
 *      @event_callback: callback for event notification
 *
 * Return:      0-success, otherwise fail
 */
int IrisEng_initiate(const char *configuration, IrisEventCB event_callback);

/**
 * Description: Release the resources used by iris engine, return 0.
 */
int IrisEng_release();

int IrisEng_release2(bool release_data);

/**
 * Description: notify engine to start enroll
 * Parameter:
 *      @user_id:   	  	the user to be enrolled
 *      @enroll_callback: 	the result returned to the caller, may call more than once
 *      @overwrite:   	  	whether overwrite the existing same user
 *                  	  	0: not overwrite, 1: overwrite the existing one
 *
 * Return:      			0-success, otherwise fail
 */
int IrisEng_enroll(const char *user_id, int overwrite, CallBack enroll_callback);


/**
 * Description: notify engine to start identify/verify
 * Parameter:
 *      @user_id:   		the user to be verified, NULL for identify
 *      @continuous: 		0: identify terminates once the result return
 *                  		1: identify never terminate until engine is told 'STOP' explicitly
 *      @identify_callback: the result returned to the caller, may call more than once
 *
 * Return:      			0-success, otherwise fail
 */
int IrisEng_identify(const char *user_id, int continuous,
                         CallBack identify_callback);


/**
 * Description: notify engine to stop, return 0
 */
int IrisEng_stop();

/**
 * Description: pause the oning action
 */
int IrisEng_pauseAction();

/**
 * Description: resume the oning action
 */
int IrisEng_resumeAction();


/**
 * Description: set preview related parameters
 * Parameter:
 *      @mode:              whole/iris
 *      @format:            preview image pixel format
 *      @width:             preview image width
 *      @height:            preview image height
 *      @callback:          preview callback
 * Return:                  0-success
 */
int IrisEng_set_preview(const char* mode, IMAGE_PIXEL_FORMAT format, int width, int height, PreviewCB callback);


int IrisEng_set_preview2(const char* mode, IMAGE_PIXEL_FORMAT format, int width, int height, int margin_x, int margin_y, PreviewCB callback);


int IrisEng_start_preview();

int IrisEng_stop_preview();


/**
 * Description: set the callback of the captured iris data
 * Parameter:
 *      @filter:            specify the iris category
 *      @callback:          callback provided by App
 * Return:                  0-success
 */
int IrisEng_set_iris_callback(IRIS_FILTER_MODE filter, RTIrisCB callback);


/**
 * Description: Add user to engine. if the user exists, overwrite it
 * Parameter:
 *      @left_iris: 		left iris's code data
 *      @right_iris: 		right iris's code data
 *      @dir_created:       1-create user data directory, 0-not create
 *
 * Return:      			0-success
 */
int IrisEng_add_user(const char *user_id,
                const unsigned char *left_iris, const unsigned char *right_iris,
                int dir_created);

/**
 * Description: Delete specified user data from engine if exists
 * Parameter:
 *      @user_id:           user id to be deleted
 *      @dir_removed:       1-remove user directory permanently, 0-not remove user directory
 * Return:                  0-success
*/
int IrisEng_delete_user(const char *user_id, int dir_removed);


/**
 * Description: Delete all user data from engine
 * Parameter:
 *      @dir_removed:       1-remove user directory permanently, 0-not remove user directory
 * Return:                  0-success
*/
int IrisEng_delete_all_user(int dir_removed);


/*
 * Description: get the directory of user data
 * Parameter:
 *      @user_dir:          out, user data directory
 * Return:                  0-success 
*/
int IrisEng_get_user_dir(char* user_dir);

/*
 * Description: get user number and list
 * Parameter:
 *      @user_num:          out, user number
 *      @list:              in/out, string of the user list, with comma as seperator
 * Return:                  0-success 

 example:
    char* l_user_list = NULL;
    int l_user_num;

    IrisEng_get_user_list(&l_user_num, NULL);
    if(l_user_num == 0)
        return;

    l_user_list = (char*)calloc(l_user_num, IRIS_USER_ID_LENGTH);
    IrisEng_get_user_list(&l_user_num, l_user_list);
*/
int IrisEng_get_user_list(int *user_num, char* list);


/*
 * Description: get user code's length
*/
int IrisEng_get_user_code_len();

/**
 * Description: fetch user's iris data
 * Parameter:
 *      @user_id:           ID of user to be fetched
 *      @left_exist:        out, indicates if left iris exists
 *      @left_iris:         pointer to left iris data
 *      @right_exist:       out, indicates if right iris exists
 *      @right_iris:        pointer to right iris data 
 * Return:      0->success, -1->user not exist
 */
int IrisEng_fetch_user(const char *user_id,
                        bool *left_exist, unsigned char *left_iris, 
                        bool *right_exist, unsigned char *right_iris);

/**
 * Description: get user's enroll data length
 * Parameter:
 *      @user_id:           ID of user to be fetched
 * Return:      data length
 *              -1 -> user not exist
 */
int IrisEng_get_enroll_data_len(const char *user_id);


/**
 * Description: fetch user's enroll data in json format
 * Parameter:
 *      @user_id:           ID of user to be fetched
 *      @data:              buffer of enroll data
 *      @len:               buffer length, ensure > IrisEng_get_enroll_data_len()
 * Return:      0 -> success
 *              -1-> user not exist
 */
int IrisEng_fetch_enroll_data(const char *user_id, char *data, int len);


int IrisEng_get_snap_data_len();

int IrisEng_fetch_snap_data(char *data, int len);

/**
 * Description: Change the configuration
 * Parameter:
 *      @configuration: the configuration string in json format, 
 *                      part of the whole configuration set is allowed
 * Return:      0-success, otherwise fail
 */
int IrisEng_change_configure(const char *configuration);


/**
 * Description: load device params
 * Parameter:
 *      @params: the configuration string in json format as below:
 {
    "E6": {
        "THD_RADIUS_L2":90,
        "THD_RADIUS_H2":135  
    },

    "C8": {
        "THD_RADIUS_L":90,
        "THD_RADIUS_H":115,
        "THD_RADIUS_L2":85,
        "THD_RADIUS_H2":120  
    }
}
 *               1st level key is always device type, e.g. "E6"
 * Return:      0-success, otherwise fail
 */
int IrisEng_load_dev_params(const char *params);


/**
 * Description: get the enroll image size
 * Parameter:
 *      @height:    image height
 *      @width:     image width 
 * Return:      0-success
 */
int IrisEng_get_image_size(int *height, int *width);


/**
 * Description: get device info
 * Return:      0-success
 */
int IrisEng_get_device_info(IrisDeviceInfo *dev_info);


/**
 * Description: get device info
 * Parameter:
 *      @dev_info:  out, buffer of device info in json-format
 *      @len:       in,  length of the buffer
 * Return:      0-success
 */
int IrisEng_get_device_info_2(char* dev_info, int len);


/**
 * Description: get iris meta
 * Parameter:
 *      @iris:       in,  iris data
 *      @iris_len:   in,  length of the iris data 
 *      @meta:       out, meta buffer in json-format
 *      @meta_len:   in,  length of the meta buffer
 * Return:      0-success
 */
int IrisEng_get_iris_meta(const unsigned char *iris, int iris_len, char* meta, int meta_len);


int IrisEng_extract_iris_code(unsigned char* image_in, int size_in, const char* format_in, unsigned char* code);


int IrisEng_check_image_quality(unsigned char* image_in, int size_in, const char* format_in, const char* method, int* quality);

/**
 * Description: get algorithm version & release date
 * Parameter:
 *      @version:    in/out,  buffer of algor version, size >= 32 bytes
 *      @date:       in/out,  buffer of release date,  size >= 32 bytes
 * Return:      0-success
 */
int IrisEng_get_algor_version(char *version, char* date);


int IrisEng_get_sdk_version(char *version, char* date);


int IrisEng_get_dev_firmware_version(char *version, char* date);


int IrisEng_update_firmware(const unsigned char* fw_bin, int len);


int IrisEng_sanity_check(CallBack check_callback);


#define IRIS_SOUND_CAPTURE                          1
#define IRIS_SOUND_POS_BACKWARD                     3
#define IRIS_SOUND_POS_FORWARD                      4
#define IRIS_SOUND_ALERT                            5
#define IRIS_SOUND_RETRY                            6
#define IRIS_SOUND_ENROLL_OK                        7
#define IRIS_SOUND_VERIFY_OK                        8
#define IRIS_SOUND_IDENTIFY_OK                      9
#define IRIS_SOUND_POS_UPWARD                       10
#define IRIS_SOUND_POS_DOWNWARD                     11
#define IRIS_SOUND_POS_LEFT                         12
#define IRIS_SOUND_POS_RIGHT                        13
#define IRIS_SOUND_ENROLL_FAIL                      17
#define IRIS_SOUND_VERIFY_FAIL                      18
#define IRIS_SOUND_IDENTIFY_FAIL                    19

typedef void (* Player)(const int sound_id);
/**
 * Description: Set sound play function
 * Parameter:
 *      @player:    sound play function
 *
 * Return:      0-success
 */
int IrisEng_set_sound_player(Player player);


void IrisEng_set_vm(void *vm);


#ifdef __cplusplus
}
#endif
#endif // __IRISENGINE_EXPORT_H__