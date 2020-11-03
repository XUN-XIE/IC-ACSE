## Flood Risk Prediction tool

### Executive Summary
The UK Environment Agency has put out a call to tender for the development of a UK Flood Risk Tool. The principal requirement of the tool is that it should be able to use three datasets to rapidly produce a list of UK postcodes in coastal areas with the highest potential flood risk, sorted by risk from highest to lowest. A secondary requirement is for the tool to link to the Environment Agency's [Rainfall API](https://environment.data.gov.uk/flood-monitoring/doc/rainfall) to allow real-time flood warning and analysis of historic flood events. The successful tender will be selected on the basis of the functionality and performance of a prototype tool in calculating the flood-risk hotspots, as well as the sophistication of additional features that exploit the API.

### Problem Specification

In environmental hazard assessment, risk is defined as the probability of a hazardous event occuring multiplied by the consequences of that event. Thus high risk can be the result of frequent events of low consequence or rare events of high consequence; the highest risk scenarios are often not obvious.

The Environment Agency routinely collects data on rainfall, sea level and river discharge and has published flooding probability maps that indicate areas of England that fall within four flood probability bands, based on the recurrence interval of flood levels that cause property damage and threat to life. These bands are very low probability (flooding expected once per 1000 years), low probability (flooding expected once per 100 years), medium probability (flooding expected once per 50 years), and high probability (flooding expected once per 10 years).

The Environment Agency has also acquired a database of total insured property value (buildings plus contents) for postcodes in UK coastal regions. This can be used to estimate the impact (financial cost) of flooding in a given postcode. In reality, flood impact will be a complex function of insured value plus the geography, property type and flood preparedness. However, for simplicity while developing the requested flood risk tool, the EA advises that it is sufficient to assume that the flood impact is given by the total insured value multiplied by a factor 0.05 (5%). 

The Environment Agency is seeking to use the flood probability and property value data in a software tool with the following functionality:

1. For an input list of UK postcodes, return the flood probability band and sort the postcodes in decreasing order of flood probability.
2. For an input list of UK postcodes, return the flood risk and sort the postcodes in decreasing order of flood risk.
3. For an input list of UK postcodes, use the Rainfall API to access live and recent rainfall data and combine with flood risk to develop a flood warning alert.
4. For a specified date, use the Rainfall API to access historical rainfall data and provide analysis of the day's rainfall.

Your challenge is to develop, document and present a prototype flood risk tool that meets these requirements. 

Requirements (1) and (2) are primary requirements that will be judged on both accuracy and performance. Performance is of concern, because the EA may wish to replace or update the flood probability data. Thus, the tool must calculate the flood probability and risk by postcode when the tool is run, rather than calculate it once and store it permanently for retrieval. 

Requirements (3) and (4) are secondary requirements that will be judged on innovation and usefulness. The secondary requirements could involve simple graphical representation of results, but do **not** require development of either a Web App or a stand-alone graphical user interface. 

The Environment Agency also requires that the prototype software tool is appropriately documented, licensed and tested. It has specified the interface of several key functions that will allow it to verify functionality and test performance and has also provided a number of simple tests as a starting point for the tool's automated test suite.

### Data sets

The Environment Agency has provided you with three datasets as the basis of your tool. One of these is a partial dataset, with the full dataset retained to assess the tool's performance

1. `postcodes.csv` A (partial) database of UK postcodes, including the latitude, longitude coordinates of the centroid of each postcode.
2. `flood_probability.csv` A database of flood probability zones. The database includes four categories of zones: very low probability, low probability, medium probability, and high probability. For this project it will be assumed that the probability zones are circular in map coordinates; the radius of each flood probability zone is also included in the database.
3. `property_value.csv` A database of approximate insured value for every postcode in UK coastal regions. The database includes postcode, latitude / longitude of postcode centroid and a nominal total insured value of properties in the postcode.

### Technical requirements

The software tool should contain the following steps to address requirements (1) and (2):

1. Retrieve postcodes from supplied database `postcodes.csv`. A partial file, for one county in England is provided for development purposes. The full UK database will be used to assess the accuracy and performance of the tool.
2. Look-up the latitudes and longitudes of the postcode centroids, provided in the postcode database.
3. Convert the latitudes and longitudes to OS Eastings and Northings, using the mathematical equations provided in the [documentation](./FloodTool.pdf).
4. For each postcode centroid, if the centroid lies within at least one flood-probability zone, find the highest flood-probability zone that contains the centroid. The flood-probability zone is assumed to be a circle in E/N coordinates with a radius specified in the `flood_probability.csv` database. 
5. Return the flood probability per postcode, as required to satisfy requirement (1).
6. Find the estimated total property value for each postcode. Combine this with the impact factor (0.05) to calculate the potential flood cost.
7. Calculate the estimated annual flood risk in pounds sterling per year.
8. Return the flood risk per postcode, as required to satisfy requirement (2).

This part of the tool will be scored for functionality and performance with the scoring framework provided in the `score` directory. The scoring algorithm can be run locally as `python -m score`, and will report timings and functionality, as well as a score estimate. Note that both the test data and the computing power for the online scoring will differ from those provided here, so this local score is proportionial to, rather than identical to, the online one. The score algorithm runs against the following API calls, for which stubs and example calls are available in the `flood_tool` directory:

 - `flood_tool.get_easting_northing_from_lat_long()`
 - `flood_tool.Tool()`
 - `flood_tool.Tool.get_lat_long()`
 - `flood_tool.Tool.get_easting_northing_flood_probability()`
 - `flood_tool.Tool.get_sorted_flood_probability()`
 - `flood_tool.Tool.get_flood_cost()`
 - `flood_tool.Tool.get_annual_flood_risk()`
 - `flood_tool.Tool.get_sorted_annual_flood_risk()`

All calls are timed, with the timing data used to adjust your tool's score.

The software tool should also contain additional functions to interface with the [EA's Rainfall API](https://environment.data.gov.uk/flood-monitoring/doc/rainfall) to address requirements (3) and (4). Note that these requirements of the tool will be judged on innovation and usefulness, as well as the quality of the code and documentation. For requirement (4) an interesting date to analyse is 6th October 2019 (for many of you, your first Sunday 
in the UK!).

Your code should have a license, installation instructions and basic documentation of the algorithms used, as method docstrings, edits to the repository's `README` file and/or as a paper manual.

### Learning Outcomes

At the end of this exercise, you should have a better understanding of:

- Collaborative software development
- The value of automated testing
- Effective teamwork

You should have reinforced your knowledge from ACSE-1 of:

- Programming with Python
- Using GitHub for collaborative software maintenance
- Using Travis CI for automated testing
- Using Pandas for data manipulation
- Using Sphinx for automated documentation

And you should be able to:

- Implement and utilise a spatial database
- Manipulate data using Python with Pandas
- Understand and use web API


### Assessment

The deadline for submission of software and all documentation is 12 pm (noon), Friday 25th October 2019.

Groups will be required to present their software and host a live demonstration on Friday afternoon (25/10/10). More details about the presentation will be provided at the project briefing.

A total of 90 marks will be awarded for the combination of software, documentation and presentation.

 - The primary requirements will be assessed on the basis of functionality and performance using objective, automated tests. Up to 30 marks will be awarded based on the accuracy of the software in returning the correct results. Up to 20 marks will be awarded based on the performance (time to return flood probability and risk results) of the tool. 
 - The secondary requirements will be assessed on innovation and usefulness, based in part on the live demonstration of the software on Friday afternoon. Up to 20 marks will be awarded for these features of the tool and the presentation.
 - Additional marks, up to a maximum of 20, will be awarded for the quality of the code, documentation and automated tests run under Travis

Note that, as with all group projects, up to an additional 10 marks will be awarded to individual students for their contribution to the project, based on the presentation/Q&A and anonymous peer-assessment conducted at the end of the project.

### Reading list

 - (A guide to coordinate systems in Great Britain)[https://webarchive.nationalarchives.gov.uk/20081023180830/http://www.ordnancesurvey.co.uk/oswebsite/gps/information/coordinatesystemsinfo/guidecontents/index.html]

 - (Information on postcode validity)[https://assets.publishing.service.gov.uk/government/uploads/system/uploads/attachment_data/file/283357/ILRSpecification2013_14Appendix_C_Dec2012_v1.pdf]

 - (The Environment Agency rainfall data API)[https://environment.data.gov.uk/flood-monitoring/doc/rainfall]
