#pragma once

namespace Interface_ns {

class StreamIO_I {
public:

    /**
     * @brief Put char
     * @param c
     * @return return the character written as an un‐signed char cast to an int or EOF on error.
     */
    virtual int putc(int c) = 0;
   /**
    * @brief Get char
    * @note Blocks if no data available or no client connected
    * @return return the character read as an unsigned char cast to an int or EOF on end of file or error.
    */
    virtual int getc() = 0;
};

}