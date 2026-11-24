#!/usr/bin/env bash
# git.sh - shortcuts for common git workflows
set -euo pipefail

MAIN_BRANCH="${GIT_MAIN_BRANCH:-main}"
REMOTE="${GIT_REMOTE:-origin}"

usage() {
    cat <<EOF
Usage: git.sh <option> [argument]

Core:
  -c,  --commit <msg>      stage everything and commit
  -C,  --commit-push <msg> stage everything, commit and push
  -m,  --main              switch to $MAIN_BRANCH
  -b,  --branch <name>     create branch, switch to it, push with upstream
       --merge <name>      merge <name> into the current branch, push, delete <name> locally and on $REMOTE

Extras:
  -p,  --push              push the current branch
  -l,  --pull              pull with --ff-only
       --sync              switch to $MAIN_BRANCH and pull
  -d,  --delete <name>     delete a branch locally and on $REMOTE
  -h,  --help              show this help

Environment: GIT_MAIN_BRANCH (default: main), GIT_REMOTE (default: origin)
EOF
}

die() {
    echo "error: $*" >&2
    exit 1
}

require_arg() {
    # $1 = option name, $2 = its argument
    if [[ -z "${2:-}" ]]; then
        die "$1 requires an argument"
    fi
}

current_branch() {
    git rev-parse --abbrev-ref HEAD
}

remote_has_branch() {
    git ls-remote --exit-code --heads "$REMOTE" "$1" >/dev/null 2>&1
}

cmd_commit() {
    git add -A
    git commit -m "$1"
}

cmd_branch() {
    git checkout -b "$1"
    git push --set-upstream "$REMOTE" "$1"
}

cmd_merge() {
    local branch="$1"
    local current
    current="$(current_branch)"

    if [[ "$branch" == "$current" ]]; then
        die "cannot merge '$branch' into itself, switch to the target branch first"
    fi

    git merge "$branch"
    git push "$REMOTE" HEAD

    if remote_has_branch "$branch"; then
        git push "$REMOTE" --delete "$branch"
    fi
    git branch -d "$branch"
}

confirm() {
    # $1 = prompt. Returns 0 only for exactly "Y", 1 for "n" or "N".
    # Re-asks on any other input. Closed stdin (EOF) counts as "no".
    local reply=""
    while true; do
        read -r -p "$1 [Y/n] " reply || return 1
        case "$reply" in
            Y)     return 0 ;;
            n|N)   return 1 ;;
            *)     echo "Please type 'Y' to confirm or 'n' to cancel." ;;
        esac
    done
}

cmd_delete() {
    local branch="$1"
    if [[ "$branch" == "$(current_branch)" ]]; then
        die "cannot delete the branch you are on"
    fi

    echo "You are about to delete branch '$branch' (locally and on $REMOTE)."
    if ! confirm "Data will be lost. Are you sure?"; then
        echo "Aborted."
        exit 1
    fi

    if remote_has_branch "$branch"; then
        git push "$REMOTE" --delete "$branch"
    fi
    git branch -d "$branch"
}

if [[ $# -eq 0 ]]; then
    usage
    exit 1
fi

case "$1" in
    -c|--commit)
        require_arg "$1" "${2:-}"
        cmd_commit "$2"
        ;;
    -C|--commit-push)
        require_arg "$1" "${2:-}"
        cmd_commit "$2"
        git push
        ;;
    -m|--main)
        git checkout "$MAIN_BRANCH"
        ;;
    -b|--branch)
        require_arg "$1" "${2:-}"
        cmd_branch "$2"
        ;;
    --merge)
        require_arg "$1" "${2:-}"
        cmd_merge "$2"
        ;;
    -p|--push)
        git push
        ;;
    -l|--pull)
        git pull --ff-only
        ;;
    --sync)
        git checkout "$MAIN_BRANCH"
        git pull --ff-only
        ;;
    -d|--delete)
        require_arg "$1" "${2:-}"
        cmd_delete "$2"
        ;;
    -h|--help)
        usage
        ;;
    *)
        usage
        die "unknown option '$1'"
        ;;
esac