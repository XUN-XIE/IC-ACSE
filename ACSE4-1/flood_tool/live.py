"""Live and historical flood monitoring data from the Environment Agency API"""
import requests
import pandas as pd
import flood_tool.geo as geo
import flood_tool.tool as tool
import numpy as np
import folium

__all__  = []

LIVE_URL = "http://environment.data.gov.uk/flood-monitoring/id/stations"
ARCHIVE_URL = "http://environment.data.gov.uk/flood-monitoring/archive/"


class get_map(object):
    '''
    class to retieve rainfall data from website to design alert system and find historical data for a specific date
    '''

    def __init__(self, date='2019-10-08'):
        self.station_url = 'http://environment.data.gov.uk/flood-monitoring/id/stations.csv?parameter=rainfall'
        self.value_url = 'http://environment.data.gov.uk/flood-monitoring/id/measures.csv?parameter=rainfall'
        self.spe_date_url = 'http://environment.data.gov.uk/flood-monitoring/data/readings.csv?parameter=rainfall&parameter=rainfall&_view=full&date=' + str(date)
        # for requirement 3
        # read the latest values from website
        # read the corresponding values of stations
        # and it is updated once the new file is uploaded
        self.date = date
        DF_value = pd.read_csv(self.value_url)
        self.value = DF_value.loc[:, ['stationReference', 'value', 'dateTime']]

        # read the correspongding latitude and longitude of stations
        DF_location = pd.read_csv(self.station_url)
        self.location = DF_location.loc[:, ['stationReference', 'lat', 'long']]

        # merge the two DataFrame to be used later.
        # the purpose is to combine the location with value
        self.DF3 = self.value.merge(self.location, left_on='stationReference', right_on='stationReference')

        DF_readings = pd.read_csv(self.spe_date_url)
        readings = DF_readings.loc[:, ['stationReference', 'value', 'dateTime']]

        DF4 = self.location.merge(readings, left_on='stationReference', right_on='stationReference')
        # turns all elements in value column into floats
        DF4['value'] = pd.to_numeric(DF4['value'], errors='coerce')
        # converts np.nan into 0
        DF4['value'] = DF4['value'].fillna(0)
        self.DF4 =DF4

    def get_all_station(self, latitude, longitude, distance):
        '''
        get corresponding rainfall value aroung each Postcode from latitude and longitude of the Postcode
        Parameters
        ----------
        latitude: sequence of float
            Ordered collection of latitude
        longitude: sequence of float
            Ordered collection of longitude

        Returns
        -------
        numpy.ndarray
            array of floats for rainfall value
        '''
        DF3 = self.DF3
        DF3['lat_max'] = DF3['lat'] + distance
        DF3['lat_min'] = DF3['lat'] - distance
        DF3['long_max'] = DF3['long'] + distance
        DF3['long_min'] = DF3['long'] - distance

        rainfall = []
        for point in range(len(latitude)):
            stations_tem = DF3.loc[(DF3.lat_max >= latitude[point]) & (DF3.lat_min <= latitude[point])]
            stations = stations_tem.loc[
                (stations_tem.long_max >= longitude[point]) & (stations_tem.long_min <= longitude[point])]

            rainfall_value = np.array(stations.value)
            rainfall_value = np.nan_to_num(rainfall_value)

            # if there is no station in the range of postcode, process the rainfall as 0
            if len(rainfall_value) == 0:
                rainfall_value = np.array([0])
            # mean = rainfall_value.sum() / len(rainfall_value)
            output = rainfall_value.max()
            rainfall.append(output)

        return rainfall





    def get_Risk_Level(self, postcode, distance):
        """
        Get an array of corresponding latitude, longitude and risk level from a sequence of postcodes
        and distance range around every postcode
        Parameters
        ----------
        postcodes: sequence of strs
            Ordered collection of postcodes
        distance: a float number

        Returns
        -------
        numpy.ndarray
            array of floats for the latitude, longitude and risk level
        """

        # create class Tool to use functions in tool.py
        # for apply function in tool.py later
        files = tool.Tool(postcode_file='flood_tool/resources/postcodes.csv', risk_file='flood_tool/resources/flood_probability.csv',
                          values_file='flood_tool/resources/property_value.csv')


        #set the range of postcode to define stations and get latitude, longitude
        lat_long = files.get_lat_long(postcode)
        latitude = lat_long[:, 0]
        longitude = lat_long[:, 1]

        #get rainfall value for input Postcode
        rainfall = np.array(self.get_all_station(latitude, longitude, distance))
        #change numpy.array to 0 which is convenient to process later
        rainfall = np.nan_to_num(rainfall)
        EOS_NOS = geo.get_easting_northing_from_lat_long(latitude, longitude)

        prob_band = files.get_easting_northing_flood_probability(EOS_NOS[0], EOS_NOS[1])

        dataset = pd.DataFrame({'Postcode': postcode, 'Latitude': latitude, 'Longitude': longitude, 'Value': rainfall, 'Probability Band': prob_band})

        #replace the probability band to threshold value
        threshold_DF= dataset.replace({'High': 1.00, 'Medium': 1.25, 'Low': 1.5, 'Very Low': 1.75, 'Zero': 2.00, 'No Risk': 2.00})

        first_level = 0.2
        second_level = 0.4
        third_level = 0.6
        def to_mark(diff):
            '''
            get the corresponding risk level as str from the difference between threshold and rainfall value

            Parameters
            ----------
            diff: a float number recording difference between threshold and rainfall value

            Returns
            -------
            str: risk level
            '''
            if diff < 0:
                result = 'No Risk'
            else:
                if diff < first_level:
                    result = 'Low Risk'
                elif (diff >= first_level) & (diff < second_level):
                    result = 'Medium Risk'
                elif (diff >= second_level) & (diff < third_level):
                    result = 'High Risk'
                else:
                    result = 'Dangerous'

            return result

        #comupute the difference between threshold and rainfall value
        diff_DF = threshold_DF.assign(diff=threshold_DF['Value'] - threshold_DF['Probability Band'])

        #change the difference to the corresponding risk level
        Risk_Ser = diff_DF['diff'].apply(to_mark)

        output = np.array(Risk_Ser)

        return latitude, longitude, output


    # work for Question 4
    def get_spe_date(self):
        """
        Get an array of corresponding latitude, longitude and risk level from a specific date

        Parameters
        ----------
        data: str as specific format
            farmat: 'yyyy-mm-dd' such as '2019-10-08'

        Returns
        -------
        numpy.ndarray
            array of floats for the latitude, longitude and risk level
        """
        files = tool.Tool(postcode_file='resources/postcodes.csv', risk_file='resources/flood_probability.csv',
                          values_file='resources/property_value.csv')
        DF4 = self.DF4
        daily_rainfall = DF4.groupby('stationReference').value.sum()
        daily_rainfall = pd.DataFrame(daily_rainfall)
        lat_long_DF = daily_rainfall.merge(self.location, left_on='stationReference', right_on='stationReference')

        latitude = np.array(lat_long_DF.lat)
        longitude = np.array(lat_long_DF.long)

        dataset = pd.DataFrame({'Latitude': latitude, 'Longitude': longitude, 'Value': lat_long_DF.value})

        first_level = 2.34
        second_level = 9.4
        third_level = 13.9

        def to_mark(value):
            '''
            get the corresponding risk level as str from the difference between threshold and rainfall value

            Parameters
            ----------
            diff: a float number recording difference between threshold and rainfall value

            Returns
            -------
            str: risk level
            '''
            if value < first_level:
                result = 'Low'
            elif (value >= first_level) & (value < second_level):
                result = 'Medium'
            elif (value >= second_level) & (value < third_level):
                result = 'High'
            else:
                result = 'Very High'

            return result

        # change the total value to the corresponding rainfall level
        Rain_level = dataset['Value'].apply(to_mark)
        output = np.array(Rain_level)

        return latitude, longitude, output



    def plot_alert_system(self, postcode):
        '''
        Get map with corresponding risk level for different postcode area from a sequence of postcodes

        Parameters
        ----------
        postcode: sequence of str

        Returns
        -------
        folium.Map
            can produce picture of map with marks
        '''
        #plot real_time data alert system map
        lat, long, level = self.get_Risk_Level(postcode, 0.1)

        tem_DF = pd.DataFrame({'lat': lat, 'long': long})
        tem_DF = tem_DF.dropna(how='any')
        lat = np.array(tem_DF.lat)
        long =np.array(tem_DF.long)
        locationlist = np.array((lat, long)).transpose()


        #the plot code must run in jupyter lab, or it cannot ouput a map
        coldict = {0: 'lightgreen', 1: 'green', 2: 'lightblue', 3: 'orange', 4: 'red'}
        #possible colours are [‘red’, ‘blue’, ‘green’, ‘purple’, ‘orange’, ‘darkred’,
        #’lightred’, ‘beige’, ‘darkblue’, ‘darkgreen’, ‘cadetblue’, ‘darkpurple’, ‘white’, ‘pink’, ‘lightblue’, ‘lightgreen’, ‘gray’, ‘black’, ‘lightgray’]

        #set the overview place at beginning to centre of England
        map = folium.Map(location=[54, -1], zoom_start=6)

        #plot the different risk level point with corresponding colorful mark
        for point in range(0, len(locationlist)):
            if level[point] == 'No Risk':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[0], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'Low Risk':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[1], fill=True, fill_opacity=0.8, radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'Medium Risk':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[2], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'High Risk':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[3], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'Dangerous':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[4], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)

        return map


    def plot_his_map(self):
        '''
        Get map with corresponding rainfall level for different stations

        Parameters
        ----------
        None

        Returns
        -------
        folium.Map
            can produce picture of map with marks
        '''
        lat, long, level = self.get_spe_date()
        tem_DF = pd.DataFrame({'lat': lat, 'long': long})
        tem_DF = tem_DF.dropna(how='any')
        lat = np.array(tem_DF.lat)
        long =np.array(tem_DF.long)
        locationlist = np.array((lat, long)).transpose()

        #the plot code must run in jupyter lab, or it cannot ouput a map
        coldict = {0: 'lightgreen', 1: 'blue', 2: 'orange', 3: 'red'}
        #possible colours are [‘red’, ‘blue’, ‘green’, ‘purple’, ‘orange’, ‘darkred’,
        #’lightred’, ‘beige’, ‘darkblue’, ‘darkgreen’, ‘cadetblue’, ‘darkpurple’, ‘white’, ‘pink’, ‘lightblue’, ‘lightgreen’, ‘gray’, ‘black’, ‘lightgray’]

        #set the overview place at beginning to centre of England
        map = folium.Map(location=[54, -1], zoom_start=6)

        #plot the different risk level point with corresponding colorful mark
        for point in range(0, len(locationlist)):
            if level[point] == 'Low':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[0], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'Medium':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[1], fill=True, fill_opacity=0.8, radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'High':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[2], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
            elif level[point] == 'Very High':
                folium.CircleMarker(locationlist[point], color='black', fill_color=coldict[3], fill=True, fill_opacity=0.8,
                                    radius=10, icon=folium.Icon(color='red')).add_to(map)
        return map
