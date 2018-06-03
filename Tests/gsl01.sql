SELECT sys.gsl_cdf_chisq_P(20.0, 5.0);  

SELECT sys.gsl_cdf_chisq_P(20.0, NULL);

SELECT sys.gsl_cdf_chisq_P(NULL, 5.0);

SELECT sys.gsl_cdf_chisq_P(-1, 5.0);

SELECT sys.gsl_cdf_chisq_P(20.0, 1.0);

drop table chi2;
CREATE TABLE chi2(a double, b double);

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0);

SELECT gsl_cdf_chisq_P(a, b) FROM chi2;

SELECT gsl_cdf_chisq_P(a, 6.0) FROM chi2; 

SELECT gsl_cdf_chisq_P(19.0, b) FROM chi2;

INSERT INTO chi2 VALUES (20.0, NULL); 

SELECT gsl_cdf_chisq_P(a, b) FROM chi2;

SELECT gsl_cdf_chisq_P(a, 6.0) FROM chi2;

SELECT gsl_cdf_chisq_P(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (NULL, 5.0);

SELECT gsl_cdf_chisq_P(a, b) FROM chi2;

SELECT gsl_cdf_chisq_P(a, 6.0) FROM chi2;

SELECT gsl_cdf_chisq_P(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (-1, 5.0);

SELECT gsl_cdf_chisq_P(a, b) FROM chi2;

SELECT gsl_cdf_chisq_P(a, 6.0) FROM chi2;

SELECT gsl_cdf_chisq_P(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (20.0, 1.0);

SELECT gsl_cdf_chisq_P(a, b) FROM chi2;

SELECT gsl_cdf_chisq_P(a, 6.0) FROM chi2;

SELECT gsl_cdf_chisq_P(19.0, b) FROM chi2;
