//
// Created by Peter Janků on 11.02.2022.
//

#ifndef KILOBOTARENA_IMAGEBUFFER_H
#define KILOBOTARENA_IMAGEBUFFER_H

/**
 * Template class for storing set of continusous data.
 * The data are stored in array - the class is working like a circular buffer. The data in buffer can be be overwrite
 * when the buffer is full or the push function can be blocking.
 * @tparam T Data type of stored values
 * @tparam size Size of circular buffer
 */
template<class T, int size>
class ImageBuffer {
public:
    /**
     * Return the size of circular buffer - amount of free bytes.
     * @return Size of buffer (elemnets of T type)
     */
    int getSize() {return size; }


    T fetchLast();

protected:
    T buffer[size];

};


#endif //KILOBOTARENA_IMAGEBUFFER_H
