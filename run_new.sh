#! /bin/bash
docker run -it --name 0aligNNlab\
    -v /home/pzw/projects/data/:/home/data:ro \
    -v /home/pzw/projects/aligNN/src/:/home/src \
    -v /home/pzw/projects/aligNN/output:/home/output \
    --cpus=8 \
    --privileged=true \
    --net=host --dns 8.8.8.8 \
    alignn:env9_22  \
    /bin/bash