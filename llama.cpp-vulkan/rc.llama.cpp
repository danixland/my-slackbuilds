#!/bin/sh

# Copyright 2026 danix <danix@danix.xyz>
# All rights reserved.
#
# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# SERVER OPTIONS
# many options can be set as env variables. 
# See https://github.com/ggml-org/llama.cpp/tree/master/tools/server

RUNAS_USER=
RUNAS_GROUP=users
LLSRV=/usr/bin/llama-server
#MODEL_DIR=
MODEL= # which model to use
HOST=127.0.0.1 # ip to bind the server to
PORT=8181 # port for the server to listen to
CONTEXT= #size of context
TEMP=1.0
TOP_K=20
TOP_P=0.95
MIN_P=0.00
SLEEP_IDLE_SECONDS=500 # after how many seconds to unload the model from memory
REASONING_FORMAT=deepseek
GPU_LAYERS=all
SPLIT_MODE=none
NO_CONTEXT_SHIFT='--no-context-shift'
LOG_FILE=/var/log/llama-server/server.log
LLAMA_ARGS="--ctx-size $CONTEXT \
            --jinja \
            --temp $TEMP \
            --top-k $TOP_K \
            --top-p $TOP_P \
            --min-p $MIN_P \
            --host $HOST --port $PORT \
            --reasoning-format $REASONING_FORMAT \
            -sm $SPLIT_MODE \
            $NO_CONTEXT_SHIFT \
            -ngl $GPU_LAYERS \
            --sleep-idle-seconds $SLEEP_IDLE_SECONDS \
            -m $MODEL" # alternatively one could set the model directory and pass --models-dir to the script

case "$1" in
        stop)
                if /usr/bin/pgrep -f "$LLSRV" >/dev/null; then
                        echo "Stopping llama-server..."
                        killall llama-server 2>/dev/null
                else
                        echo "llama-server is not running..."
                        exit 1
                fi
                ;;
        start)
                echo "Starting llama-server..."
                if [[ -d $(dirname $LOG_FILE) ]]; then
                    mkdir -p $(dirname $LOG_FILE)
                    touch $LOG_FILE
                    chown -R ${RUNAS_USER}:${RUNAS_GROUP} $(dirname $LOG_FILE)
                fi
                su $RUNAS_USER -c "$LLSRV $LLAMA_ARGS --log-file $LOG_FILE -lv 3 --log-timestamps" 1>/dev/null 2>&1 &
                $0 status
                ;;
        restart)
                $0 stop
                sleep 1
                $0 start
                ;;
        status)
                if /usr/bin/pgrep -f "$LLSRV" >/dev/null; then
                        echo "llama-server is running"
                        echo "Browser interface is at:"
                        echo "http://${HOST}:${PORT}"
                fi
                ;;
        *)
                echo "usage: $0 { start | stop | status | restart }" >&2
                exit 1
                ;;
esac
