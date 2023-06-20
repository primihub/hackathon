#!/bin/bash
docker-compose -f docker-compose-etcdraft2.yaml up -d
docker-compose -f docker-compose-cli.yaml up -d
