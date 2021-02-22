# syntax=docker/dockerfile:1.2
FROM node:15.8.0-alpine3.12 as base
ENV NODE_ENV=production

FROM base as builder
WORKDIR /app/
COPY [".", "/app/"]
COPY ["id_rsa", "config", "/root/.ssh/"]
RUN --mount=type=cache,target=/root/.npm apk add --update --no-cache --virtual .gyp python3 make g++ git cmake openssh-client \
			&& chmod 0600 /root/.ssh/id_rsa && ssh-keyscan -H github.com >> /root/.ssh/known_hosts && eval `ssh-agent -s` && ssh-add /root/.ssh/id_rsa \
			&& npm i -g --production node-gyp npm-check-updates npm@7.5.4 \
			&& npm ci --only=production \			
			&& npm prune --production && npm cache clean --force && rm -Rf /app/build/Release/obj.target /app/build/Release/.deps /app/src && apk del .gyp

CMD [ "/bin/sh" ]
