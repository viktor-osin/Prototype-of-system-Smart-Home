//======================================================authorship
//by Viktor Osin (2015)
//code-based Michael Osthege (2013)
//======================================================
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using TCD.Controls;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using TCD.Arduino.Bluetooth;
using Windows.UI.Popups;
using Windows.Foundation;
using System.Threading.Tasks;



namespace SmartHome
{
    /// <summary>
    /// IMPORTANT:
    /// in Package.appxmanifest make sure that you declare the following capabilities:
    /*
  ...
  <Capabilities>
    <m2:DeviceCapability Name="bluetooth.rfcomm">
      <m2:Device Id="any">
        <m2:Function Type="name:serialPort" />
      </m2:Device>
    </m2:DeviceCapability>
  </Capabilities>
  ...
</Package>
     */
    /// </summary>

    /* Before you can connect a Bluetooth device to Windows (Phone) 8, you have to go to the Bluetooth settings
     * (Win+I\PC Settings\Devices\Bluetooth) and pair the device. Probably you have to enter a PIN, which is often 1234
     * 
     * After you've paired the device, you can launch the BluetoothCommunicationSampleController app and tap "Connect"
     */


    public sealed partial class MainPage : Page
    {
         private byte notPass;
         private byte korON;
         private byte vanON;
         private byte gostON;
         private byte perON;
         private byte ventilON;
         private byte zanavON;
         private byte AllON;
         private byte auoON; 
         private byte auzON; 
         private byte auvON; 
         private byte sigON; 
         private byte soON;
         private byte ainfON; 
         private byte iamready;

        public MainPage()
        {
            this.InitializeComponent();
            App.BluetoothManager.MessageReceived += BluetoothManager_MessageReceived;
            App.BluetoothManager.ExceptionOccured += BluetoothManager_ExceptionOccured;
           
         /*   Kor.IsEnabled = false;
            Gost.IsEnabled = false;
            Van.IsEnabled = false;
            Per.IsEnabled = false;
            All.IsEnabled = false;
            checkLight.IsChecked = true;
            checkVentil.IsChecked = true;
            checkZanav.IsChecked = true;
            Inform.IsOn = true;
            Ventil.IsOn = false;
         //   Zanav.IsEnabled = false;
            Zanav.IsOn = false;*/
        }
        protected override void OnNavigatedFrom(Windows.UI.Xaml.Navigation.NavigationEventArgs e)
        {
            App.BluetoothManager.Disconnect();//отключает блютуз
        }
        private void WriteLine(string line)//прописывает новую линию в консоли
        {
            console.Text += line + "\n";
            scrollViewer.ChangeView(0, scrollViewer.ScrollableHeight, 1, false);
         
        }

        #region Bluetooth Connection Lifecycle
        //подключение
        private async void BluetoothConnect_Click(object sender, RoutedEventArgs e)
        {
            await App.BluetoothManager.EnumerateDevicesAsync((sender as Button).GetElementRect());//показывает устройства, готовые к подключению
        }

        private async void BluetoothManager_ExceptionOccured(object sender, Exception ex)
        {
            var md = new MessageDialog(ex.Message, "Похоже, возникли проблемы с Bluetooth подключением:(");
            md.Commands.Add(new UICommand("Эх.. Спасибо.."));
            md.DefaultCommandIndex = 0;
            var result = await md.ShowAsync();
        }        
        #endregion

        #region Send & Receive
        
        private async void BluetoothManager_MessageReceived(object sender, string message)
        {

            int equIndex = message.IndexOf('=');
            if (equIndex > 0)//is this a report? (eg: "A0=245" alternative: confirmation like "RED_ON")
            {
                switch (message.Substring(0, equIndex))//we can receive more than one report...
                {
                    case "IN": int nom = Convert.ToInt32(message.Substring(equIndex + 1)); 
                       // WriteLine("Температура: " +nom +" градусов"); 
                        break;
                 
                }
            }
            else
            {
                switch (message) //получить сообщение     
                {
                    case "I_AM_READY_END": WriteLine("Константы состояния системы обновлены!"); iamready = 0; break;

                    case "AUO_ON": WriteLine("Автоматическое освещение включено"); auoON = 1; checkLight.IsChecked = true; break;
                    case "AUO_OFF": WriteLine("Ручное управление светом"); auoON = 0; checkLight.IsChecked = false; break;

                    case "AUZ_ON": WriteLine("Автоматическое управление занавесками"); auzON = 1; checkZanav.IsChecked = true; break;
                    case "AUZ_OFF": WriteLine("Ручное управление занавесками"); auzON = 0; checkZanav.IsChecked = false; break;

                    case "AUV_ON": WriteLine("Автоматическое управление вентиляцией"); auvON = 1; checkVentil.IsChecked = true; break;
                    case "AUV_OFF": WriteLine("Ручное управление вентиляцией"); auvON = 0; checkVentil.IsChecked = false; break;

                    case "SIG_ON": WriteLine("Охранный режим включен"); notPass = 1; Security.IsOn = true; break;
                    case "SIGNAL_OFF": WriteLine("Охранный режим выключен");
                        notPass = 0;
                        Security.IsOn = false;
                        checkLight.IsEnabled = true;
                        checkZanav.IsEnabled = true;
                        checkVentil.IsEnabled = true;
                        if (iamready == 0) { checkLight.IsChecked = true; }  //константа предотвращает включение автом. света при обновлении интерфейса
                        break;

                    case "KORIDOR_ON": WriteLine("Свет в коридоре включен"); korON = 1; Kor.IsOn = true; break;
                    case "KORIDOR_OFF": WriteLine("Свет в коридоре выключен"); korON = 0; Kor.IsOn = false; break;

                    case "VAN_ON": WriteLine("Свет в ванной включен"); vanON = 1; Van.IsOn = true; break;
                    case "VAN_OFF": WriteLine("Свет в ванной выключен"); vanON = 0; Van.IsOn = false; break;

                    case "GOST_ON": WriteLine("Свет в гостиной включен"); gostON = 1; Gost.IsOn = true; break;
                    case "GOST_OFF": WriteLine("Свет в гостиной выключен"); gostON = 0; Gost.IsOn = false; break;

                    case "PERIM_ON": WriteLine("Свет по периметру включен"); perON = 1; Per.IsOn = true; break;
                    case "PERIM_OFF": WriteLine("Свет по периметру выключен"); perON = 0; Per.IsOn = false; break;

                    case "ALL_ON": WriteLine("Свет включен во всех комнатах"); AllON = 1; All.IsOn = true; break;
                    case "ALL_OFF": WriteLine("Свет выключен во всех комнатах"); AllON = 0; All.IsOn = false; break;

                    case "SO_ON": WriteLine("Серверное отделение открыто"); soON = 1; SO.IsOn = true; break;
                    case "SO_OFF": WriteLine("Серверное отделение закрыто"); soON = 0; SO.IsOn = false;  break;

                    case "AUTOINF_ON": WriteLine("Автоинформатор включен"); ainfON = 1; Inform.IsOn = true; break;
                    case "AUTOINF_OFF": WriteLine("Автоинформатор выключен"); ainfON = 0; Inform.IsOn = false; break;

                    case "ZANAV_OPEN": WriteLine("Занавески открыты"); zanavON = 1; Zanav.IsOn = true; break;
                    case "ZANAV_CLOSE": WriteLine("Занавески закрыты"); zanavON = 0; Zanav.IsOn = false; break;

                    case "VENTIL_ON": WriteLine("Вентиляция включена"); ventilON = 1; Ventil.IsOn = true; break;
                    case "VENTIL_OFF": WriteLine("Вентиляция выключена"); ventilON = 0; Ventil.IsOn = false; break;
                }
            }
           // WriteLine("< " + message);  //показать входящую информацию  
        }
        #endregion

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            string cmd = "1"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }
        private async void Button_Click2(object sender, RoutedEventArgs e)
        {
            string cmd = "1"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }
        private async void Button_Click1(object sender, RoutedEventArgs e)
        {
            string cmd = "1"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }   
        private async void Button_Click3(object sender, RoutedEventArgs e)
        {
            string cmd = "3"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }
        private async void Button_Click4(object sender, RoutedEventArgs e)
        {
            string cmd = "4"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }
        private async void Button_Click5(object sender, RoutedEventArgs e)
        {
            string cmd = "7"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }
        private async void Button_Click18(object sender, RoutedEventArgs e)
        {
            string parol = Passw1.Text;
            var res = await App.BluetoothManager.SendMessageAsync(parol);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + parol);
        }

        #region Серверное отделение
        private async void ToggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (soON == 0)
                    {
                        string cmd = "9"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (soON == 1)
                    {
                        string cmd = "10"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }
        #endregion

        private async void ToggleSwitch_Toggled_Kor(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (korON == 0)
                    {
                        string cmd = "11"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (korON == 1)
                    {
                        string cmd = "22"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        private async void ToggleSwitch_Toggled_Gost(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (gostON == 0)
                    {
                        string cmd = "12"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (gostON == 1)
                    {
                        string cmd = "21"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        private async void ToggleSwitch_Toggled_Van(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (vanON == 0)
                    {
                        string cmd = "13"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (vanON == 1)
                    {
                        string cmd = "31"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        private async void ToggleSwitch_Toggled_Per(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (perON == 0)
                    {
                        string cmd = "14"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (perON == 1)
                    {
                        string cmd = "41"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        private async void ToggleSwitch_Toggled_All(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    korON = 1; gostON = 1; vanON = 1;
                    Kor.IsOn = true;
                    Gost.IsOn = true;
                    Van.IsOn = true;
                    if (AllON == 0)
                    {
                        string cmd = "15"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }

                }
                else
                {
                    korON = 0; gostON = 0; vanON = 0;
                    Kor.IsOn = false;
                    Gost.IsOn = false;
                    Van.IsOn = false;
                    if (AllON == 1)
                    {
                        string cmd = "51"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                   
               }
            }
        }

        private async void ToggleSwitch_Security(object sender, RoutedEventArgs e)
        {
              ToggleSwitch toggleSwitch = sender as ToggleSwitch;
              if (toggleSwitch != null)
              {
                  if (toggleSwitch.IsOn == true)
                  {
                      if (notPass == 0)
                      {
                          string cmd = "4"; // посылаемое сообщение в serial
                          var res = await App.BluetoothManager.SendMessageAsync(cmd);
                          if (res == 1)//прибавление знака > к посланному сообщению
                              WriteLine("Охранный режим включен");
                      }
                      notPass = 1;
                      Kor.IsEnabled = false;
                      Gost.IsEnabled = false;
                      Van.IsEnabled = false;
                      Per.IsEnabled = false;
                     // All.IsOn = false;
                      All.IsEnabled = false;
                      gostON = 0;// Gost.IsOn = false;
                      korON = 0; //Kor.IsOn = false;
                      vanON = 0; //Van.IsOn = false;
                      checkLight.IsEnabled = false;
                      checkZanav.IsEnabled = false;
                      checkVentil.IsEnabled = false;
                  }


                  else
                  {
                      if (notPass == 1)
                      {
                          var md = new MessageDialog("Что бы отключить охранный режим необходимо поднести RFID-карту к считывателю, либо ввести пароль..");
                          md.Commands.Add(new UICommand("Хорошо"));
                          md.DefaultCommandIndex = 0;
                          var result = await md.ShowAsync();
                          Security.IsOn = true;
                      } 
                  }
              }
        }
       

        #region Автоинформатор
        private async void ToggleSwitch_Toggled_Inf(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (ainfON == 0)
                    {
                        string cmd = "6"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (ainfON == 1)
                    {
                        string cmd = "60"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }
        #endregion
        #region Панель помощи и оценки
        private void HelpPage(object sender, RoutedEventArgs e)
        {
            SmartHome.HelpSettingsFlyout helpSF = new SmartHome.HelpSettingsFlyout();
            helpSF.ShowIndependent();

        }


        private void SendMassage(object sender, RoutedEventArgs e)
        {
            SmartHome.SendSettingsFlyout helpSF = new SmartHome.SendSettingsFlyout();
            helpSF.ShowIndependent();
        }
         #endregion

        private void AppBarButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void disconnect_Click(object sender, RoutedEventArgs e)
        {

        }

        private async void Button_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            string cmd = "255"; // посылаемое сообщение в сериал
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }

        private async void ToggleSwitch_Ventil(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (ventilON == 0)
                    {
                        string cmd = "16"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (ventilON == 1)
                    {
                        string cmd = "61"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        private async void ToggleSwitch_Zanav(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (zanavON == 0)
                    {
                        string cmd = "17"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
                else
                {
                    if (zanavON == 1)
                    {
                        string cmd = "71"; // посылаемое сообщение в сериал
                        var res = await App.BluetoothManager.SendMessageAsync(cmd);
                        if (res == 1)//прибавление знака > к посланному сообщению
                            WriteLine("> " + cmd);
                    }
                }
            }
        }

        #region Ввод температуры
        private async void Button_Click19(object sender, RoutedEventArgs e)
        {
            string call = "5";
            var mas = await App.BluetoothManager.SendMessageAsync(call);
            await Task.Delay(1000);
            string cmd = Temp.Text;
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
            if (res == 1)//прибавление знака > к посланному сообщению
                WriteLine("> " + cmd);
        }

        #endregion

        #region Управление светом
        private async void checkLight_Checked(object sender, RoutedEventArgs e)
        {
            if (auoON == 0)
            {
                WriteLine("Автоматическое освещение включено");
                string cmd = "3"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)//прибавление знака > к посланному сообщению
                    WriteLine("> " + cmd);
                auoON = 1;
            }
            
            Kor.IsEnabled = false;
            Gost.IsEnabled = false;
            Van.IsEnabled = false;
            Per.IsEnabled = false;
            All.IsEnabled = false;
        }
       

        private async void checkLight_unchecked(object sender, RoutedEventArgs e)
        {
            if (auoON == 1)
            {
                WriteLine("Ручное управление освещением");
                string cmd = "1"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)//прибавление знака > к посланному сообщению
                    WriteLine("> " + cmd);
            }
            Kor.IsEnabled = true;
            Gost.IsEnabled = true;
            Van.IsEnabled = true;
            Per.IsEnabled = true;
            All.IsEnabled = true;
        }

        #endregion

        #region Управление занавесками
        private async void checkZanav_Checked(object sender, RoutedEventArgs e)
        {
            if (auzON == 0)
            {
                WriteLine("Автоматическое управление занавесками");
                string cmd = "8"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)
                WriteLine("> " + cmd);
                auzON = 1;
            }
            Zanav.IsEnabled = false;   
        }

        private async void checkZanav_unchecked(object sender, RoutedEventArgs e)
        {
            if (auzON == 1)
            {
                WriteLine("Ручное управление занавесками");
                string cmd = "80"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)
                WriteLine("> " + cmd);
                auzON = 0;
            }
            Zanav.IsEnabled = true;  
        }
        #endregion

        #region Управление вентиляцией
        private async void checkVentil_Checked(object sender, RoutedEventArgs e)
        {
            if (auvON == 0)
            {
                WriteLine("Автоматическое управление вентиляцией");
                string cmd = "2"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)//прибавление знака > к посланному сообщению
                    WriteLine("> " + cmd);
                auvON = 1;
            }
            Ventil.IsEnabled = false;
        
        }


        private async void checkVentil_unchecked(object sender, RoutedEventArgs e)
        {
            if (auvON == 1)
            {
                WriteLine("Ручное управление вентиляцией");
                string cmd = "20"; // посылаемое сообщение в сериал
                var res = await App.BluetoothManager.SendMessageAsync(cmd);
                if (res == 1)//прибавление знака > к посланному сообщению
                    WriteLine("> " + cmd);
                auvON = 0;
            }
            Ventil.IsEnabled = true;
        }
        #endregion

        private async void interf_update(object sender, RoutedEventArgs e)
        {
            string cmd = "99"; // посылаемое сообщение в сериал
            iamready = 1;
            var res = await App.BluetoothManager.SendMessageAsync(cmd);
                var md = new MessageDialog("Запрос на обновление интерфейса приложения отправлен..");
                md.Commands.Add(new UICommand("Спасибо"));
                md.DefaultCommandIndex = 0;
                var result = await md.ShowAsync();
        }







    }
}