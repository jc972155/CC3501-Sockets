#include <curl/curl.h>
#include <stdio.h>
#include <fcntl.h>
#include <string>

// To call we use ./http_client google.com

size_t http_callback(void *buffer, size_t sz, size_t nmemb, void *userp)
{
    /*
    This callback function gets called by libcurl as soon as there
    is data received that needs to be saved. buffer points to the delivered
    data, and the size of that data is sz multiplied with nmemb.

    The callback function will be passed as much data as possible in all
    invokes, but you must not make any assumptions. It may be one byte, it may
    be thousands. The maximum amount of body data that will be passed to the
    write callback is defined in the curl.h header file: CURL_MAX_WRITE_SIZE
    (the usual default is 16K). If CURLOPT_HEADER is enabled, which makes header
    data get passed to the write callback, you can get up to
    CURL_MAX_HTTP_HEADER bytes of header data passed into it. This usually means
    100K.

    This function may be called with zero bytes data if the transferred file is
    empty.

    The data passed to this function will not be zero terminated!
    */

    /* Calculate the amount of data that was passed in */
    size_t size = sz * nmemb;

    // Was data received?
    if (size > 0) {
        // Print the message that was received.
        // It will not be null terminated, so use fwrite instead of printf.
        fwrite(buffer, sz, nmemb, stdout);
        printf("\n");
    } else {
        printf("Received an empty response.\n");
    }

    return size;
}

int main(int argc, char *argv[])
{
    // argc is the number of command-line arguments provided to the program.
    // The first argument (argv[0]) is always the name of the program.
    if (argc < 3) {
        printf("Usage:\n");
        printf("%s Username and Message\n", argv[0]);
        return 1;
    }

    std::string username = argv[1];
    std::string message = argv[2];

    // Initialise the HTTP library
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Failed to initialise the curl library\n");
        return 1;
    }

    char *user = curl_easy_escape(curl, username.c_str(), username.length());
    char *data = curl_easy_escape(curl, message.c_str(), message.length());

    std::string url = "http://api.thingspeak.com/update?api_key=ZKE95ZURWV7DW8B0&field1=" + std::string(user) + "&field2=" + std::string(data);

    curl_free(user);
    curl_free(data);

    // Set the callback function that will receive the actual data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_callback);

    // Configure the URL to load
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Send the HTTP request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Done
    curl_easy_cleanup(curl);
    return 0;
}
