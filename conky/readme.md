# [ CONKY CONFIG ]

My conky config is usually very simple / minimal. I also don't chnage it very often, but when I do you'll find it here....

## Single line pipe..

This is a single line that is set to output to console. The output is then piped to a txt file in /tmp/ which is then displayed in xfce4-panel using [**awk**](https://linux.die.net/man/1/awk) & [**xfce4-genmon-plugin.**](https://goodies.xfce.org/projects/panel-plugins/xfce4-genmon-plugin) The screenshot below shows my full desktop, but zoom in on the panel to get a closer look at my conky config. The script that I use during startup to pipe conky to a txt file can be found in my scripts folder labeled "conkypanel"....

> Conky piped to xfce4-panel using awk & xfce4-genmon-plugin.

![alt text](http://i.imgur.com/4wr1XXm.png "Conky displayed inside xfce4-panel")

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

###### [ PLEASE NOTE THAT FOR THE UPDATES PART TO WORK YOU MUST GIVE "UPDATES.SH" ROOT ACCESS ]

----

## Minimal top-right..

A minimal theme that's set to the top-right of the screen..

> Minimal conky showing at the top-right of screen.

![alt text](http://i.imgur.com/YSZHyy5.png "Conky displaying top-right of screen")

There's no setup required for this one. Just run conky as normal....
