# 2018微算機專題 Music Maker (組別:28)

<h1><a href=""https://youtu.be/JIX0cNqM830>DEMO</a></h1>

- 專題動機與願景
    - 看到google song maker後決定實作透過互動讓音樂初學者或兒童認識音調跟節奏的特性的作品
- 系統功能與原理說明 : 
    - (參照下方架構圖區)
- 系統使用環境及對象 : 
    - server為UNIX based OS environment，網頁為任何電腦/手機皆可使用。
    - 對象為任何對音樂感性去的使用者皆可
- 系統完整架構圖、流程圖、電路圖、設計圖 : 
    - 架構圖
    ![](https://i.imgur.com/fyTRGem.png)
    - 流程圖
    ![](https://i.imgur.com/fS58bVA.png)
    - 電路圖
    ![](https://i.imgur.com/yofXbip.png)
    - 設計圖
    ![](https://i.imgur.com/m3ftSUv.jpg)

- 系統開發工具、材料及技術 :
    - 系統開發工具 : nodejs, MPLAB
    - 材料 : LED、蜂鳴器、Rpi3
    - 技術 : C, Javascript, HTML, AJAX
- 周邊接口或 Library 及 API 使用說明 : 
    - server library/API : nodejs/ npm serialport
    - 使用說明 : sudo node server.js (port設定12345)
- 實際組員之分工項目 : 
    - 林慶瑞 : GUI設計, server架設, 線路改善(降低雜訊/接觸不良等可能性), 作品外觀設計, debug
    - 蔡鎮偉 : 提案、pic18 code改善、線路設計、外觀設計、美工、debug
    - 彭佳緯 : pic18 code實作、線路設計、外觀設計、debug
- 預期效益與結語 : 
    - 希望使用者可以透過我們的作品對音樂有更深的認識，作出一首自己喜歡的曲子。

