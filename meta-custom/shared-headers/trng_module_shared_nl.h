#ifndef __TRNG_MODULE_SHARED_NL_H__
#define __TRNG_MODULE_SHARED_NL_H__

/**
 * @brief annonymous enum with generic netlink attributes which are mapped on appropriate value
 * 
 */
enum
{
    ATTR_UNSPEC,       /*!< unspecific attribute which are necessarry because only non zero attribute can be mapped */
    ATTR_CTRL_WORD_ID, /*!< attr for control word ID */
    ATTR_WORDS,        /*!< attr for additional words form user*/
    ATTR_RESPONSE,     /*!< attr for response to user */
    __ATTR_COUNT
};
#define ATTR_MAX (__ATTR_COUNT - 1) /*<! number of created attributes */

/**
 * @brief annonymous enum for command flags which are mapped to a appropriate function
 * 
 */
enum
{
    CMD_UNSPEC,               /*<! unspecific attribute which are necessarry because only non zero command can be mapped*/
    CMD_HANDLE_MSG_FROM_USER, /*!< command because of which supervisor function will be called */
    __CMD_COUNT
};
#define CMD_MAX (__CMD_COUNT - 1) /*!< number of commands created command flags*/

#define ALIVE 0x01
#define GET_RANDOM 0x02
#define SET_LED_MODE 0x03
#define GET_LED_MODE 0x04
#define TEST_SET_RANDOM_MODE 0x70
#define TEST_SET_RANDOM_PATTERN 0x71
#define FAMILY_NAME "embedded_c"

#endif // __TRNG_MODULE_SHARED_NL_H__
