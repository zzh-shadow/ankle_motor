#ifndef __USR_CONFIG_H__
#define __USR_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "app_pubinclude.h"


struct USR_CONFIG_s;


void usr_config_set_default_config(struct USR_CONFIG_s *usr_config);
int usr_config_read_config(struct USR_CONFIG_s *usr_config);
int usr_config_write_config(struct USR_CONFIG_s *usr_config);
int usr_config_set_control_mode(struct USR_CONFIG_s *usr_config , uint8_t mode);



#ifdef __cplusplus
}
#endif




#endif