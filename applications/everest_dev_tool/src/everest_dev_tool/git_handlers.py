import argparse
import subprocess

def clone_handler(args: argparse.Namespace):
    log = args.logger

    log.info(
        f"Cloning repository:\n"
        f"  Method: {args.method}\n"
        f"  Host: {args.host}\n"
        f"  SSH User (if ssh): {args.ssh_user}\n"
        f"  Organization: {args.organization}\n"
        f"  Repository Name: {args.repository_name}\n"
        f"  Branch: {args.branch}\n"
    )
    repository_url = ""
    if args.method == 'https':
        repository_url = f"https://{args.host}/"
    else:
        repository_url = f"{args.ssh_user}@{args.host}:"
    repository_url = repository_url + f"{ args.organization }/{ args.repository_name }.git"

    cmd_args = ["git", "clone", "-b", args.branch, repository_url]

    log.debug(f"Command to execute: {' '.join(cmd_args)}")

    if args.dry:
        log.info(f"Dry run: Would execute: {' '.join(cmd_args)}")
    else:
        subprocess.run(cmd_args, check=True)
