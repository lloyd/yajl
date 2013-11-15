#include <stdio.h>
#include <string.h>

#include "yajl/yajl_gen.h"


/**
 * function to output an error as a string instead of a status code
 * */
char * statusToString(yajl_gen_status code) {

    switch (code) {


        case yajl_gen_status_ok:
            return "no error";
            break;

        case yajl_gen_keys_must_be_strings:
            return "at a point where a map key is generated, a function other then yajl_gen_string was called";
            break;

        case yajl_max_depth_exceeded:
            return "YAJL's maximum generation depth was exeeded, see YAJL_MAX_DEPTH";
            break;

        case yajl_gen_in_error_state:
            return "a generator function was called while in an error state";
            break;

        case yajl_gen_generation_complete:
            return "a complete JSON document has already been generated (tried to add elements after the top level container was closed";
            break;

        case yajl_gen_invalid_number:
            return "an invalid number was passed in (infinity or NaN)";
            break;

        case yajl_gen_no_buf:
            return "a print callback was passed in, so no internal buffer to get from";
            break;

        case yajl_gen_invalid_string:
            return "an invalid string was passed in to yajl_gen_string() (yajl_gen_validate_utf8 option is enabled)"; 
            break;

        default:
            return "invalid code!";
            break;

    }

}

/**
 * prints an error message given a code
 * */
void err(yajl_gen_status code) {

    printf("an error occured while generating the json: %s\n", statusToString(code));
}


int main(int argc, const char* argv[]) {


    yajl_gen handle = yajl_gen_alloc(NULL);

    if (handle == NULL) {

        printf("Could not allocate yajl_gen handle!");

        return 1;
    }

    // here we are going to be recreating a list of 'objects' that was returned
    // by the now defunct twitter public timeline. See the included 'twitter_public_timeline.json'
    // file for the original json. 


    // for each yajl_gen_* call, it returns a yajl_gen_status code, which tells us
    // if something went wrong or not. However yajl does not save this code, so 
    // if one does not check it every time, you could possibly miss an error 
    // and yajl will continue happily. 

    yajl_gen_status stat = yajl_gen_status_ok;

    // open top level object, an array
    stat = yajl_gen_array_open(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // inside the array are 'objects' for each twitter post
    stat = yajl_gen_map_open(handle);  if (stat != yajl_gen_status_ok) { err(stat); return 1;} 
    
    // start generating fields
    
    // 'geo'
    stat = yajl_gen_string(handle, "geo", strlen("geo")); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // 'in_reply_to_user_id'
    stat = yajl_gen_string(handle, "in_reply_to_user_id", strlen("in_reply_to_user_id")); 
    if (stat != yajl_gen_status_ok) { err(stat); return 1;} 
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 
    
    // 'in_reply_to_status_id'
    stat = yajl_gen_string(handle, "in_reply_to_status_id", strlen("in_reply_to_status_id"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;} 
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // truncated
    stat = yajl_gen_string(handle, "truncated", strlen("truncated")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // source
    stat = yajl_gen_string(handle, "source", strlen("source")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "web", strlen("web")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'favorited'
    stat = yajl_gen_string(handle, "favorited", strlen("favorited")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'created at'
    stat = yajl_gen_string(handle, "created_at", strlen("created_at")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "Wed Nov 04 07:20:37 +0000 2009", strlen("Wed Nov 04 07:20:37 +0000 2009"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // 'in_reply_to_screen_name'
    stat = yajl_gen_string(handle, "in_reply_to_screen_name", strlen("in_reply_to_string_name"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_null(handle);

    // 'user', value is an object
    stat = yajl_gen_string(handle, "user", strlen("user"));
    stat = yajl_gen_map_open(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/notifications'
    stat = yajl_gen_string(handle, "notifications", strlen("notifications"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/favourites_count'
    stat = yajl_gen_string(handle, "favourites_count", strlen("favourites_count"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 0);

    // 'user/description'
    stat = yajl_gen_string(handle, "description", strlen("description"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "AdMan / Music Collector", strlen("AdMan / Music Collector"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/following'
    stat = yajl_gen_string(handle, "following", strlen("following")); 
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/statuses_count'
    stat = yajl_gen_string(handle, "statuses_count", strlen("statuses_count"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 617); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_text_color'
    stat = yajl_gen_string(handle, "profile_text_color", strlen("profile_text_color"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "8C8C8C", strlen("8C8C8C"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/geo_enabled'
    stat = yajl_gen_string(handle, "geo_enabled", strlen("geo_enabled"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_background_image_url'
    stat = yajl_gen_string(handle, "profile_background_image_url", strlen("profile_background_image_url"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    char * bgImgUrl = "http://s.twimg.com/a/1257288876/images/themes/theme9/bg.gif";
    stat = yajl_gen_string(handle, bgImgUrl, strlen(bgImgUrl));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_image_url'
    stat = yajl_gen_string(handle, "profile_image_url", strlen("profile_image_url"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    char *imgUrl = "http://a3.twimg.com/profile_images/503330459/madmen_icon_normal.jpg";
    stat = yajl_gen_string(handle, imgUrl, strlen(imgUrl));

    // 'user/profile_link_color'
    stat = yajl_gen_string(handle, "profile_link_color", strlen("profile_link_color"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "2FC2EF", strlen("2FC2EF"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/verified'
    stat = yajl_gen_string(handle, "verified", strlen("verified"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_background_tile'
    stat = yajl_gen_string(handle, "profile_background_tile", strlen("profile_background_tile"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/url'
    stat = yajl_gen_string(handle, "url", strlen("url")); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_null(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/screen_name'
    stat = yajl_gen_string(handle, "screen_name", strlen("screen_name"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "khaled_itani", strlen("khaled_itani"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/created_at'
    stat = yajl_gen_string(handle, "created_at", strlen("created_at"));   
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    char * createdAt = "Thu Jul 23 20:39:21 +0000 2009";
    stat = yajl_gen_string(handle, createdAt, strlen(createdAt));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_background_color'
    stat = yajl_gen_string(handle, "profile_background_color", strlen("profile_background_color"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "1A1B1F", strlen("1A1B1F"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_sidebar_fill_color'
    stat = yajl_gen_string(handle, "profile_sidebar_fill_color", strlen("profile_sidebar_fill_color"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "252429", strlen("252429"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // 'user/followers_count'
    stat = yajl_gen_string(handle, "followers_count", strlen("followers_count"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 156); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/protected'
    stat = yajl_gen_string(handle, "protected", strlen("protected"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_bool(handle, 0); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/location"
    stat = yajl_gen_string(handle, "location", strlen("location"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "Tempe, Arizona", strlen("Tempe, Arizona"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // 'user/name'
    stat = yajl_gen_string(handle, "name", strlen("name"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "Khaled Itani", strlen("Khaled Itani"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // 'user/time_zone"
    stat = yajl_gen_string(handle, "time_zone", strlen("time_zone"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "Pacific Time (US & Canada)", strlen("Pacific Time (US & Canada)"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // 'user/friends_count'
    stat = yajl_gen_string(handle, "friends_count", strlen("friends_count"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 151); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/profile_sidebar_border_color'
    stat = yajl_gen_string(handle, "profile_sidebar_border_color", strlen("profile_sidebar_border_color"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_string(handle, "050505", strlen("050505"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/id'
    stat = yajl_gen_string(handle, "id", strlen("id"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 59581900); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'user/utc_offset"
    stat = yajl_gen_string(handle, "utc_offset", strlen("utc_offset"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, -28800); if (stat != yajl_gen_status_ok) { err(stat); return 1;}
 
    // close the 'user' object
    stat = yajl_gen_map_close(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'id'
    stat = yajl_gen_string(handle, "id", strlen("id"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    stat = yajl_gen_integer(handle, 5414922107); if (stat != yajl_gen_status_ok) { err(stat); return 1;}

    // 'text'
    stat = yajl_gen_string(handle, "text", strlen("text"));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    char * textStr = "RT @cakeforthought 24. If you wish hard enough, you will hear your current favourite song on the radio minutes after you get into your car.";
    stat = yajl_gen_string(handle, textStr, strlen(textStr));
    if (stat != yajl_gen_status_ok) { err(stat); return 1;}
    
    // close object (for this individual twitter post)
    stat = yajl_gen_map_close(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // close top level list
    stat = yajl_gen_array_close(handle); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // get the buffer of the json we generated
    const unsigned char * buf;
    size_t len;
    stat = yajl_gen_get_buf(handle, &buf, &len); if (stat != yajl_gen_status_ok) { err(stat); return 1;} 

    // now we can do something with the generated json!
    printf("%s\n", buf);


    return 0;
}



