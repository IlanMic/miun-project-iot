/* minimal CoAP server
 *
 * Copyright (C) 2018-2021 Olaf Bergmann <bergmann@tzi.org>
 */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <coap3/coap.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

//Get the temperature from the raspberry
 float get_temperature()
{
    string file_name = "/sys/class/thermal/thermal_zone0/temp";
    ifstream temp_raspb_file;
    float temp_raspb = 0.0f;
    stringstream buff;

    //we read the temperature of the raspberr's CPU and put it in the buffer
    temp_raspb_file.open(file_name);
    buff << temp_raspb_file.rdbuf();
    temp_raspb_file.close();

    //convert the value of the temperature from string to float
    temp_raspb = stof(buff.str());

    cout << stof(buff.str()) << endl;

    //convert the value of the temperature in celsius
    temp_raspb = temp_raspb / 1000.0; 
    cout << "Measured value: " << temp_raspb << endl;
    return temp_raspb;
}

//Convert a float value to a char pointer
const char* convert_float_to_char_ptr(float temperature)
{
    string temperature_raspberry;
    const char* temp_rpi;
    temperature_raspberry = to_string(temperature);
    temp_rpi = temperature_raspberry.c_str();
    cout << "Converted string value: " << temp_rpi << endl;
    return temp_rpi;
}

int resolve_address(const char *host, const char *service, coap_address_t *dst) {

  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  int error, len=-1;

  memset(&hints, 0, sizeof(hints));
  memset(dst, 0, sizeof(*dst));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;

  error = getaddrinfo(host, service, &hints, &res);

  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
    case AF_INET6:
    case AF_INET:
      len = dst->size = ainfo->ai_addrlen;
      memcpy(&dst->addr.sin6, ainfo->ai_addr, dst->size);
      goto finish;
    default:
      ;
    }
  }

 finish:
  freeaddrinfo(res);
  return len;
}

int main(void) {
  coap_context_t  *ctx = nullptr;
  coap_address_t dst;
  coap_resource_t *resource = nullptr;
  coap_endpoint_t *endpoint = nullptr;
  int result = EXIT_FAILURE;;
  coap_str_const_t *ruri = coap_make_str_const("temp_cpu");
  coap_startup();
  const char * temperature_rpi = convert_float_to_char_ptr(get_temperature());
  cout << "First measured value: " << *(temperature_rpi) << endl;

  /* resolve destination address where server should be sent */
  if (resolve_address("localhost", "5683", &dst) < 0) {
    coap_log(LOG_CRIT, "failed to resolve address\n");
    goto finish;
  }

  /* create CoAP context and a client session */
  ctx = coap_new_context(nullptr);

  if (!ctx || !(endpoint = coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP))) {
    coap_log(LOG_EMERG, "cannot initialize context\n");
    goto finish;
  }

  resource = coap_resource_init(ruri, 0);
  coap_register_handler(resource, COAP_REQUEST_GET,
                        [](auto, auto,
                           const coap_pdu_t *request,
                           auto,
                           coap_pdu_t *response) {
                          coap_show_pdu(LOG_WARNING, request);
                          coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
                          coap_add_data(response, 5,
                                        (const uint8_t *)convert_float_to_char_ptr(get_temperature()));
                          coap_show_pdu(LOG_WARNING, response);
                        });
  coap_add_resource(ctx, resource);
  while(true){ 
    coap_delete_resource(ctx, resource);
    resource = coap_resource_init(ruri, 0);
    coap_register_handler(resource, COAP_REQUEST_GET,
                          [](auto, auto,
                            const coap_pdu_t *request,
                            auto,
                            coap_pdu_t *response) {
                            coap_show_pdu(LOG_WARNING, request);
                            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
                            coap_add_data(response, 5,
                                          (const uint8_t *)convert_float_to_char_ptr(get_temperature()));
                            coap_show_pdu(LOG_WARNING, response);
                          });
    coap_add_resource(ctx, resource);
    sleep(1);
    coap_io_process(ctx, COAP_IO_WAIT);  
  }

  result = EXIT_SUCCESS;
 finish:

  coap_free_context(ctx);
  coap_cleanup();

  return result;
}