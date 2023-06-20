#!/bin/bash
docker-compose -f docker-compose-etcdraft2.yaml down
docker-compose -f docker-compose-cli.yaml down
