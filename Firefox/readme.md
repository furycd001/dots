# [ FIREFOX USERCHROME.CSS ]

These are Firefox usercChrome.css that I have created. They are meant to be minimal & with the colors being based on the ever popular [**arc theme.**](https://github.com/horst3180/Arc-theme)

## Arc-proton..
![alt text](https://i.imgur.com/71OMjZx.png "Arc")

#### Close buttons are hidden inside favicons.
![alt text](https://imgur.com/Y2spqJY.png "Close Button")

#### New Tab background color changed with the css posted at the bottom of the page.
![alt text](https://imgur.com/Zpmgni6.png "New Tab Page")

#### Changing the background color of new tabs.

Just add the code below to your userContent.css file. It's that simple.

```css
/* Newtab */
/* Newtab */
@-moz-document url("about:blank") {
    body {
        background-color: #272b35 !important;
    }
}

@-moz-document url("about:newtab") {
          body {
              background-color: #272b35 !important;
          }
      }
```

> I made this userchrome more minimal by pairing it with this **["ARC DARK"](https://addons.mozilla.org/en-GB/firefox/addon/arc-dark-theme-we/)** theme. Consider installing it for the userchrome to work correctly....

## Arc-old..
![alt text](https://i.imgur.com/2RUYodQ.jpg "Arc")

#### Close buttons are hidden inside favicons.
![alt text](https://i.imgur.com/WYLxKFN.jpg "Close Button")

#### New Tab background color changed with the css posted at the bottom of the page.
![alt text](https://i.imgur.com/wQtjAzZ.jpg "New Tab Page")

#### Tabs are hidden if there is only one tab open.
![alt text](https://i.imgur.com/19ZPHS6.jpg "Tabs Hidden")

> Firefox arc was originally converted from my old [**Waterfox userchrome.css.**](https://github.com/furycd001/dots/tree/master/waterfox) I wanted to still keep everything minimal, but have it be more useable. As always there's still a few things that need worked on, but it really is pretty complete as it is now & I'm super happy using it daily.

## Webview..
![alt text](https://i.imgur.com/8xr8q34.jpg "Webview")

> This is a super minimal userchrome that has everything except the webview hidden. You can access the urlbar by pressing **CTRL+L** & switch between tabs using **CTRL+PGUP & CTRL+PGDN**.

## Single..
![alt text](https://i.imgur.com/iIMwjBA.jpg "Single")

> Everything you should need is squished onto a single row. This is totally usable until you randomly decide to open like a gazillion tabs.
>  
> [ PLEASE NOTE THAT THIS IS CURRENTLY BROKEN AS OF VER.88/89 ]
>  
> [ I MAY FIX THIS IN THE FUTURE ]

----

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/Z8Z44445F)
