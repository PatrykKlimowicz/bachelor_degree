#ifndef __TRNG_MODULE_SIM_H_
#define __TRNG_MODULE_SIM_H_

#include <trng_module_nl.h>

/**
 * @brief This function simulates sending data via SPI to dev. Supervisor call this function.
 * 
 * @param msg pointer to Msg structure which contains all neccessary data.
 */
void trng_module_sim_send(Msg *msg);

/**
 * @brief simulates receiving data from dev. Supervisor call this function.
 * 
 * @param msg pointer to Msg structure which contains all neccessary data.
 * @param buf buffer which will be filled with data
 */
void trng_module_sim_receive(Msg *msg, void *buf);

#endif /* __TRNG_MODULE_SIM_H_ */
