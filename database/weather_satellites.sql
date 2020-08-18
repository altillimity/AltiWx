--
-- PostgreSQL database dump
--

-- Dumped from database version 11.7 (Debian 11.7-0+deb10u1)
-- Dumped by pg_dump version 11.7 (Debian 11.7-0+deb10u1)

-- Started on 2020-08-10 19:41:10 BST

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 197 (class 1259 OID 16394)
-- Name: satellites; Type: TABLE; Schema: public; Owner: altiwx
--

CREATE TABLE public.satellites (
    norad integer NOT NULL,
    min_elevation integer NOT NULL,
    priority integer NOT NULL,
    downlinks jsonb
);


ALTER TABLE public.satellites OWNER TO altiwx;

--
-- TOC entry 2900 (class 0 OID 16394)
-- Dependencies: 197
-- Data for Name: satellites; Type: TABLE DATA; Schema: public; Owner: altiwx
--

COPY public.satellites (norad, min_elevation, priority, downlinks) FROM stdin;
25338	10	1	{"APT": {"type": "FM", "radio": "wx2m", "doppler": false, "bandwidth": 42000, "frequency": 137620000, "output_extension": "wav", "processing_script": "apt-noaa.py", "modem_audio_sample_rate": 11025}, "DSB": {"type": "IQ", "radio": "wx2m", "doppler": false, "bandwidth": 48000, "frequency": 137350000, "output_extension": "raw", "processing_script": "dsb-noaa.py"}}
28654	10	1	{"APT": {"type": "FM", "radio": "wx2m", "doppler": false, "bandwidth": 42000, "frequency": 137912500, "output_extension": "wav", "processing_script": "apt-noaa.py", "modem_audio_sample_rate": 11025}, "DSB": {"type": "IQ", "radio": "wx2m", "doppler": false, "bandwidth": 48000, "frequency": 137350000, "output_extension": "raw", "processing_script": "dsb-noaa.py"}}
33591	10	1	{"APT": {"type": "FM", "radio": "wx2m", "doppler": false, "bandwidth": 42000, "frequency": 137100000, "output_extension": "wav", "processing_script": "apt-noaa.py", "modem_audio_sample_rate": 11025}, "DSB": {"type": "IQ", "radio": "wx2m", "doppler": false, "bandwidth": 48000, "frequency": 137770000, "output_extension": "raw", "processing_script": "dsb-noaa.py"}}
40069	10	1	{"LRPT": {"type": "IQ", "radio": "wx2m", "doppler": false, "bandwidth": 140000, "frequency": 137100000, "output_extension": "raw", "processing_script": "lrpt-meteorm2.py"}}
\.


--
-- TOC entry 2778 (class 2606 OID 16401)
-- Name: satellites satellites_pkey; Type: CONSTRAINT; Schema: public; Owner: altiwx
--

ALTER TABLE ONLY public.satellites
    ADD CONSTRAINT satellites_pkey PRIMARY KEY (norad);


-- Completed on 2020-08-10 19:41:10 BST

--
-- PostgreSQL database dump complete
--


