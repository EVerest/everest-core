#!/bin/sh

DEPLOYED_DOCS_REPO="$EXT_MOUNT/build/docs/deployed_docs_repo"

mkdir -p ~/.ssh
ssh-keyscan github.com >> ~/.ssh/known_hosts
chmod 600 ~/.ssh/known_hosts

git -C "$DEPLOYED_DOCS_REPO" config user.email "compiler@pionix.de"
git -C "$DEPLOYED_DOCS_REPO" config user.name "Pionix Github Service Account"

git -C "$DEPLOYED_DOCS_REPO" add .
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Staging changes failed with return code $retVal"
    exit $retVal
fi

git -C "$DEPLOYED_DOCS_REPO" commit -m "Update nightly documentation from commit $GITHUB_SHA"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Committing changes failed with return code $retVal"
    exit $retVal
fi

git -C "$DEPLOYED_DOCS_REPO" push
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Pushing changes failed with return code $retVal"
    exit $retVal
fi
