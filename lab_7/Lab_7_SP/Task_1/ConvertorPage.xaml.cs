using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace Task_1
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ConvertorPage : Page
    {
        // lenght constants
        const double METER_TO_MILE_COEFF = 0.0006214;
        const double METER_TO_YARD_COEFF = 1.094;
        const double METER_TO_INCH_COEFF = 39.37;
        const double METER_TO_FOOT_COEFF = 3.281;
        // weight constants
        const double KILOGRAM_TO_POUND_COEFF = 2.204623;
        const double KILOGRAM_TO_OUNCE_COEFF = 35.27396;
        // volume constants
        const double LITTER_TO_PINT_COEFF = 2.113376;
        const double LITTER_TO_GALLON_COEFF = 0.264172;

        public ConvertorPage()
        {
            this.InitializeComponent();

            ColapseAllConvertors();
            Lenght_grid.Visibility = Visibility.Visible;
        }

        private void FromMetricLenghtConversion_Click(object sender, RoutedEventArgs e)
        {
            if (double.TryParse(meters_tbox.Text, out double meters) && meters >= 0)
            {
                miles_tbox.Text = (meters * METER_TO_MILE_COEFF).ToString();
                yards_tbox.Text = (meters * METER_TO_YARD_COEFF).ToString();
                inch_tbox.Text = (meters * METER_TO_INCH_COEFF).ToString();
                foot_tbox.Text = (meters * METER_TO_FOOT_COEFF).ToString();
            }
            else
            {
                ShowMsgDialog("Error", "No data to convertsion! Please, input data in meters field");
            }
        }

        private void FromMetricidthConversion_Click(object sender, RoutedEventArgs e)
        {
            // bring all metic values to kilograms 
            double resKg = 0;
            if (double.TryParse(kg_tbox.Text, out double kg) && kg >= 0)
            {
                resKg += kg;
            }
            if (double.TryParse(gram_tbox.Text, out double gr) && gr >= 0)
            {
                resKg += (gr * 0.001);
            }
            if (double.TryParse(centner_tbox.Text, out double cen) && cen >= 0)
            {
                resKg += cen*100;
            }
            if (double.TryParse(tonne_tbox.Text, out double tn) && tn >= 0)
            {
                resKg += tn*1000;
            }

            if (resKg < 0)
            {
                ShowMsgDialog("Error", "No data to convertsion! Please, input data in width fields");
                return;
            }

            // convert to pounds, ounces
            pound_tbox.Text = (resKg * KILOGRAM_TO_POUND_COEFF).ToString();
            ounce_tbox.Text = (resKg * KILOGRAM_TO_OUNCE_COEFF).ToString();
        }

        private void FromMetricVolumeConversion_Click(object sender, RoutedEventArgs e)
        {
            // bring all metic values to litters
            double resLt = 0;
            if (double.TryParse(liter_tbox.Text, out double lt) && lt >= 0)
            {
                resLt += lt;
            }
            if (double.TryParse(cCentim_tbox.Text, out double cCent) && cCent >= 0)
            {
                resLt += (cCent * 0.001);
            }
            if (double.TryParse(cMeter_tbox.Text, out double cMt) && cMt >= 0)
            {
                resLt += cMt * 1000;
            }

            if (resLt < 0)
            {
                ShowMsgDialog("Error", "No data to convertsion! Please, input data in volume fields");
                return;
            }

            // convert to pounds, ounces
            pint_tbox.Text = (resLt * LITTER_TO_PINT_COEFF).ToString();
            gallon_tbox.Text = (resLt * LITTER_TO_GALLON_COEFF).ToString();
        }

        private void BackToMainPage_btn_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.GoBack();
        }

        private async void ShowMsgDialog(string title, string content)
        {
            ContentDialog noDataNotif = new ContentDialog()
            {
                Title = title,
                Content = content,
                CloseButtonText = "OK"
            };
            await noDataNotif.ShowAsync();
        }

        private void Menu_btn_Click(object sender, RoutedEventArgs e)
        {
            mode_spliteView.IsPaneOpen = true;
        }

        private void ShowLenghtConv_btn_Click(object sender, RoutedEventArgs e)
        {
            ColapseAllConvertors();
            Lenght_grid.Visibility = Visibility.Visible;
            mode_spliteView.IsPaneOpen = false;
        }

        private void ShowWidthConv_btn_Click(object sender, RoutedEventArgs e)
        {
            ColapseAllConvertors();
            Width_grid.Visibility = Visibility.Visible;
            mode_spliteView.IsPaneOpen = false;
        }

        private void ShowVolumeConv_btn_Click(object sender, RoutedEventArgs e)
        {
            ColapseAllConvertors();
            Volume_grid.Visibility = Visibility.Visible;
            mode_spliteView.IsPaneOpen = false;
        }

        private void ColapseAllConvertors()
        {
            Lenght_grid.Visibility = Visibility.Collapsed;
            Width_grid.Visibility = Visibility.Collapsed;
            Volume_grid.Visibility = Visibility.Collapsed;
        }
    }
}
