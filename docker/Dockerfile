#
# Setting up OS and packages.
#
FROM alpine:3.8
RUN echo http://nl.alpinelinux.org/alpine/edge/testing >> /etc/apk/repositories
RUN apk add --no-cache numactl numactl-dev make g++ libc6-compat file zlib zlib-dev
RUN mkdir /coquiaas

#
# Adding runsolver.
#
ADD runsolver_src /coquiaas/runsolver_src
RUN make -C /coquiaas/runsolver_src/src/ -j
RUN mv /coquiaas/runsolver_src/src/runsolver /coquiaas/runsolver
RUN rm -rf /coquiaas/runsolver_src

#
# Adding CoQuiAAS.
#
ADD coquiaas-2.0 /coquiaas/coquiaas-2.0
RUN cd /coquiaas/coquiaas-2.0/ && ./configure && make
RUN mv /coquiaas/coquiaas-2.0/bin/CoQuiAAS /coquiaas/CoQuiAAS
RUN rm -rf /coquiaas/coquiaas-2.0/

#
# Adding LBX.
#
# ADD lbx/lbx /coquiaas/lbx

#
# Adding wrapper.sh and setting it as entrypoint.
#
COPY wrapper.sh /coquiaas/wrapper.sh
ENTRYPOINT ["/coquiaas/wrapper.sh"]
