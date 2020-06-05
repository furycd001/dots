# [ FIREFOX USERCHROME.CSS ]

This is my Firefox usercChrome.css. It's based on the ever popular [**arc theme**](https://github.com/horst3180/Arc-theme) & converted from my old [**Waterfox userchrome.css**](https://github.com/furycd001/dots/tree/master/waterfox). I wanted to still keep minimal looks in mind, but be more useable. As always there's still a few things that need worked on, but it really is pretty complete as it is now & I'm super happy using it daily.

#### Firefox Arc.
![alt text](http://i.imgur.com/4rHkNKF.png "Firefox Arc")

#### Close buttons are hidden inside favicons.
![alt text](http://i.imgur.com/0rrxdkc.png "Close Button")

#### New Tab background color changed with the css posted at the bottom of the page.
![alt text](http://i.imgur.com/H0sWd2z.png "New Tab Page")

#### Tabs are hidden if there is only one tab open.
![alt text](http://i.imgur.com/qhJ9ZoP.png "Tabs Hidden")


#### Changing the background color of new tabs.

Just add the code below to your userContent.css file. It's that simple.

```css
/* Newtab */
@-moz-document url("about:blank") {
    body {
        background-color: #272b35 !important;
    }
}
```
