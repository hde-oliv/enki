FROM	    ubuntu:focal

RUN	        apt-get update ; \
            apt-get install -y \
            libmbedtls-dev \
            clang \
            llvm \
            make \
            dumb-init \
            libmongoc-1.0-0 \
            libmongoc-dev

WORKDIR	    /usr/include
RUN		    ln -s libmongoc-1.0/ mongoc; ln -s libbson-1.0/ bson

WORKDIR	    /enki
COPY        . .

EXPOSE      80

ENTRYPOINT  [ "make", "run" ]
