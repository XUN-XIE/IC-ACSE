"""Locator functions to interact with geographic data"""
import numpy as np
import pandas as pd
import flood_tool.geo as geo

__all__ = ['Tool']


def clean_postcodes(postcodes):
    """
    Takes list or array of postcodes, and returns it in a cleaned numpy array
    """
    postcode_df = pd.DataFrame({'Postcode':postcodes})
    postcode_df['Postcode'] = postcode_df['Postcode'].str.upper()

    # If length is not 7 get rid of spaces. This fixes e.g. "SW19 2AZ" -> "SW192AZ"
    postcode_df['Postcode'] = postcode_df['Postcode'].where(
        postcode_df['Postcode'].str.len() == 7, postcode_df['Postcode'].str.replace(" ", ""))

    # If length is 5 (e.g. "W67HZ") add two spaces in the middle (-> "W6  7HZ")
    postcode_df['Postcode'] = postcode_df['Postcode'].where(
        postcode_df['Postcode'].str.len() != 5,
        postcode_df['Postcode'].str[:2]+ "  " + postcode_df['Postcode'].str[2:])

    # If length is 6 (e.g. "SW72AZ") add a space in the middle and end(-> "SW7 2AZ")
    postcode_df['Postcode'] = postcode_df['Postcode'].where(
        postcode_df['Postcode'].str.len() != 6,
        postcode_df['Postcode'].str[:3]+ " " + postcode_df['Postcode'].str[3:])

    return postcode_df['Postcode'].to_numpy()


class Tool(object):
    """Class to interact with a postcode database file."""

    def __init__(self, postcode_file=None, risk_file=None, values_file=None):
        """

        Reads postcode and flood risk files and provides a postcode locator service.

        Parameters
        ---------

        postcode_file : str, optional
            Filename of a .csv file containing geographic location data for postcodes.
        risk_file : str, optional
            Filename of a .csv file containing flood risk data.
        values_file : str, optional
            Filename of a .csv file containing property value data for postcodes.
        """
        self.postcode_file = postcode_file
        self.risk_file = risk_file
        self.values_file = values_file

        self.postcode_df = pd.read_csv(self.postcode_file)

        # Make data frame of values & clean the postcodes in them.
        self.values_df = pd.read_csv(self.values_file)
        postcode_arr = self.values_df['Postcode'].to_numpy()
        postcode_arr = clean_postcodes(postcode_arr)
        self.values_df['Postcode'] = postcode_arr

        # Make data frame of risks, add columns to be used in get...flood_probability
        self.risk_df = pd.read_csv(self.risk_file)
        # Northing_max (:= northing+radius), northing_min, easting_max, easting_min for each row
        self.risk_df["X_max"] = self.risk_df["X"] + self.risk_df["radius"]
        self.risk_df["X_min"] = self.risk_df["X"] - self.risk_df["radius"]
        self.risk_df["Y_max"] = self.risk_df["Y"] + self.risk_df["radius"]
        self.risk_df["Y_min"] = self.risk_df["Y"] - self.risk_df["radius"]
        # Also add column of radius squared r2
        self.risk_df["radius_squared"] = np.square(self.risk_df["radius"])


    def get_lat_long(self, postcodes):
        """Get an array of WGS84 (latitude, longitude) pairs from a list of postcodes.

        Parameters
        ----------

        postcodes: sequence of strs
            Ordered sequence of N postcode strings

        Returns
        -------

        ndarray
            Array of Nx_2 (latitude, longitdue) pairs for the input postcodes.
            Invalid postcodes return [`numpy.nan`, `numpy.nan`].
        """
        # Fix evil postcodes
        postcodes = clean_postcodes(postcodes)

        postcode_df = self.postcode_df
        postcode_df = postcode_df.fillna('np.nan')
        postcode_df = postcode_df.set_index('Postcode')
        index_data = postcode_df.loc[postcodes]
        lat = np.array(index_data['Latitude']).T
        lng = np.array(index_data['Longitude']).T

        return np.vstack((lat, lng)).transpose()


    def get_easting_northing_flood_probability(self, easting, northing):
        """Get an array of flood risk probabilities from arrays of eastings and northings.
        Flood risk data is extracted from the Tool flood risk file. Locations
        not in a risk band circle return `Zero`, otherwise returns the name of the
        highest band it sits in.
        Parameters
        ----------
        easting: numpy.ndarray of floats
            OS Eastings of locations of interest
        northing: numpy.ndarray of floats
            Ordered sequence of postcodes
        Returns
        -------
        numpy.ndarray of strs
            numpy array of flood probability bands corresponding to input locations.
        """
        # Read in risk files as pandas dataframe
        risks = self.risk_df
        prob_bands = np.full(np.size(easting), "Zero", dtype='<U8')

        # For each point we get:
        for point, point_east in enumerate(easting):
            point_north = northing[point]
            # Pick the zones where easting_min < easting < easting_max
            zones = risks.loc[(risks.X_max >= point_east) & (risks.X_min <= point_east)]
            # Further reduce these to where northing_min < northing < northing_max
            zones_pot = zones.loc[(zones.Y_max >= point_north) & (zones.Y_min <= point_north)]
            # For each potential zone:
            for i in range(len(zones_pot.index)):
                # Don't bother with further zones if we already know the risk is High
                if prob_bands[point] == "High":
                    break

                row = zones_pot.iloc[i]
                # Squared distance from point to zone (we use squares to avoid square-rooting)
                dist2 = (row.X-point_east)*(row.X-point_east) + (row.Y-point_north)*(row.Y-point_north)
                if dist2 <= row.radius_squared:
                    risk = row.prob_4band
                    current_band = prob_bands[point]
                    if risk == "High":
                        prob_bands[point] = risk
                    elif risk == "Medium" and current_band != "High":
                        prob_bands[point] = risk
                    elif risk == "Low" and (current_band != "High" and current_band != "Medium"):
                        prob_bands[point] = risk
                    elif risk == "Very Low" and current_band == "Zero":
                        prob_bands[point] = "Very Low"

        return prob_bands


    def get_sorted_flood_probability(self, postcodes):
        """Get an array of flood risk probabilities from a sequence of postcodes.
        Probability is ordered High>Medium>Low>Very low>Zero.
        Flood risk data is extracted from the `Tool` flood risk file.
        Parameters
        ----------
        postcodes: sequence of strs
            Ordered sequence of postcodes
        Returns
        -------
        pandas.DataFrame
            Dataframe of flood probabilities indexed by postcode and ordered from `High` to `Zero`,
            then by lexagraphic (dictionary) order on postcode. The index is named `Postcode`, the
            data column is named `Probability Band`. Invalid postcodes and duplicates
            are removed.
        """
        # Fix evil postcodes
        postcodes = clean_postcodes(postcodes)

        # Get latitude and longitude
        output = self.get_lat_long(postcodes)  # Returns latitude,longitude pairs in an array
        lat_long = pd.DataFrame(
            {'Postcode':postcodes, 'latitude':output[:, 0], 'longitude':output[:, 1]})

        # Delete the wrong format of postcode
        lat_long = lat_long.dropna(how='any')
        latitude = np.array(lat_long.latitude)
        longitude = np.array(lat_long.longitude)

        # Returns Eastings and Northings in an array
        output_2 = geo.get_easting_northing_from_lat_long(latitude, longitude)

        # Returns array of flood risk probabilities
        output_3 = self.get_easting_northing_flood_probability(output_2[0], output_2[1])

        # New column in dataframe containing the probabilities
        lat_long['Probability Band'] = output_3

        # Removing invalid postcodes
        lat_long = lat_long.dropna(how='any')
        # Removing duplicates
        lat_long = lat_long.drop_duplicates(subset='Postcode')

        # Sort by Probability Bands
        # add variable ordered to sort later by Xun Xie
        lat_long['Probability Band'] = pd.Categorical(
            lat_long['Probability Band'],
            categories=["High", "Medium", "Low", "Very Low", "Zero"], ordered=True)
        #add sort firstly by Probability Band and then sort secondly by Postcode
        lat_long = lat_long.sort_values(by=['Probability Band', 'Postcode'], ascending=[True, True])
        lat_long = lat_long.set_index('Postcode')

        return lat_long  # Make Postcode the Index


    def get_flood_cost(self, postcodes):
        """Get an array of estimated cost of a flood event from a sequence of postcodes.
        Parameters
        ----------
        postcodes: sequence of strs
            Ordered collection of postcodes
        Returns
        -------
        numpy.ndarray of floats
            array of floats for the pound sterling cost for the input postcodes.
            Invalid postcodes return `numpy.nan`.
        """
        # Fix evil postcodes
        postcodes = clean_postcodes(postcodes)

        values_df = self.values_df[['Postcode', 'Total Value']]
        values_df = values_df.loc[values_df.Postcode.isin(postcodes)]
        values_df = values_df.set_index('Postcode').reindex(postcodes)
        values_df = values_df.fillna(0)

        return np.array(values_df['Total Value'])


    def get_annual_flood_risk(self, postcodes, probability_bands):
        """Get an array of estimated annual flood risk in pounds sterling per year of a flood
        event from a sequence of postcodes and flood probabilities.
        Parameters
        ----------
        postcodes: sequence of strs
            Ordered collection of postcodes
        probability_bands: sequence of strs
            Ordered collection of flood probabilities
        Returns
        -------
        numpy.ndarray
            array of floats for the annual flood risk in pounds sterling for the input postcodes.
            Invalid postcodes return `numpy.nan`.
        """
        #get cost_value
        cost_value = self.get_flood_cost(postcodes)

        #create Dataframe for replacing corresonding value
        risk_df = pd.DataFrame({'Probability Band': probability_bands})
        total_df = risk_df.replace(
            {'High':0.1, 'Medium': 0.02, 'Low': 0.01, 'Very Low': 0.001, 'Zero': 0})
        pro_ser = np.array(total_df['Probability Band'])

        #compute result
        annual = pro_ser * cost_value * 0.05

        return  annual


    def get_sorted_annual_flood_risk(self, postcodes):
        """Get a sorted pandas DataFrame of flood risks.

        Parameters
        ----------

        postcodes: sequence of strs
            Ordered sequence of postcodes

        Returns
        -------

        pandas.DataFrame
            Dataframe of flood risks indexed by (normalized) postcode and ordered by risk,
            then by lexagraphic (dictionary) order on the postcode. The index is named
            `Postcode` and the data column `Flood Risk`.
            Invalid postcodes and duplicates are removed.
        """

        # Fix evil postcodes
        postcodes = clean_postcodes(postcodes)

        # Get lat, long of postcodes
        arr = self.get_lat_long(postcodes)
        lat = arr[:, 0] # Latitude
        lng = arr[:, 1] # Longitude

        # Convert lat, long -> easting, northing
        tem = geo.get_easting_northing_from_lat_long(lat, lng, radians=False)
        eos = tem[0]    # Easting
        nos = tem[1]    # Northing

        # Get our data frame of postcodes and risks
        prob_band = self.get_easting_northing_flood_probability(eos, nos)
        flood_risk = self.get_annual_flood_risk(postcodes, prob_band)
        risk_df = pd.DataFrame({'Postcode':postcodes, 'Flood Risk':flood_risk})

        # Clean up data frame
        risk_df = risk_df.drop_duplicates()
        risk_df = risk_df.set_index('Postcode')
        risk_df = risk_df.sort_values(by=['Flood Risk', 'Postcode'], ascending=[False, True])

        return risk_df
