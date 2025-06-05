# [ BASH ]

Bash related stuff....

## prompt..
![alt text](bash_prompt.webp "bash prompt")
```bash
PS1='\n\[\e[34m\]┌─\[\e[0m\] \[\e[32m\]\w\[\e[0m\]\n\[\e[34m\]└─\[\e[0m\]   '
```


## Function to remove orphaned packages..

```bash
rdup() {
    while [[ $(pacman -Qdtq) ]]; do
        echo "The following packages will be removed:"
        pacman -Qdtq
        read -p "Proceed? (y/N): " confirm
        if [[ $confirm != [yY] ]]; then
            echo "Aborted."
            return
        fi
        sudo pacman -Rsn $(pacman -Qdtq)
    done
    echo "No orphaned packages to be removed."
}
```
