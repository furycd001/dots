# [ CONKY ]

My conky config's are usually simple & minimal. I don't really change my config very often, but when I do you'll be able to find it here....

## Single line pipe..

This is a single line that is set to output to console. The output is then piped to a txt file in /tmp/ which is then displayed in xfce4-panel using [**awk**](https://linux.die.net/man/1/awk) & [**xfce4-genmon-plugin.**](https://goodies.xfce.org/projects/panel-plugins/xfce4-genmon-plugin) The screenshot below shows my full desktop, but zoom in on the panel to get a closer look at my conky config. The script that I use during startup to pipe conky to a txt file can be found in my scripts folder labeled "conkypanel"....

> Conky piped to xfce4-panel using awk & xfce4-genmon-plugin.

![alt text](http://i.imgur.com/y7w5Mdo.png "Conky displayed inside xfce4-panel")

#### How to set up....  

Start by installing conky & xfce4-genmon-plugin. On debian based systems you can use “apt” from the command line or search for both using either synaptic or software centre. Both can be installed on any Linux system using the default package manager of that system. I just happen to be running xubuntu.

> sudo apt install conky xfce4-genmon-plugin

Once you have both of those apps installed on your system go ahead and create a single line conky config. Once you've got that created and saved, open up and modify your conky config file to make conky output to the terminal only. This can be done by adding the following two lines to your config file.

> out_to_x no  
> out_to_console yes  

Save and close your config file once you've added those two lines above.  

Next you'll want to set conky to output to a txt file on startup. This can be done easily by launching conky with the following command.

> conky -q -c /your/path/to/conkyconfig > /tmp/conky.txt

You can add that above command to a bash script and set it to run on startup command from within your “Application Autostart” in xfce. First just change the path to wherever your conky.conf is located.

The final step is to add xfce4-genmon-plugin to your panel and then use awk to print the txt file to your panel.

> awk 'END{print}' /tmp/conky.txt

That's it. You should now have conky displaying within your panel. If not then start from the top and double check everything.

----

## Top-right..

A minimal conky that sit's at the top-right of the screen..

> Conky showing at the top-right of screen.

![alt text](https://i.imgur.com/GxUPFMA.jpg "Conky displaying top-right of screen")

There's no setup required for this one. Just run conky as normal....

----

## External..

Similar to top right only set to display on my external monitor & also show top 4 processes..

> Conky displaying on external monitor.

![alt text](https://imgur.com/ZaB6eQQ.png "Conky displaying top-right of screen")

The last update part at the bottom works for any arch based distro, but I'm pretty sure you could get it working with Debian, Fedora or any other distro....

----

## Terminal..

Running conky once inside a terminal..

> Minimal conky displaying inside a notification.

![alt text](https://imgur.com/PKjCur3.png "Conky displaying inside a terminal window")

This conky is super simple & also super useful. Add it to a bash_alias & easily run it from anywhere, be it x or tty. You can also download my _"conkyterminal"_ script & combine it with a keybinding & the command below....

```bash
xfce4-terminal -H --hide-menubar --geometry=38x7 --execute /home/furycd001/Dots/Gucci/conkyterminal.sh &
```

----

## Lock..

Similar to top right only set to display on my external monitor & also show top 4 processes..

> Conky displaying on external monitor.

![alt text](https://i.imgur.com/fAjpvS3.png "Conky displaying in center of external monitor")

Originaly created to use with a lockscreen, but never got it working. Still like this config though so kept it. Again using **"xinerama_head"** for the external monitor....

----

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/Z8Z44445F)
