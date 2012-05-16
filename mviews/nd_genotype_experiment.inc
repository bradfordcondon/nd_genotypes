<?php

/** 
 * Register a Materialized View for stock=>genotype=>feature data
 * 
 * This allows for quicker querying of data by doing the majority of the
 * JOINS ahead of time.
 *
 * Each row in this table corresponds to a genotype observed in a stock. 
 * Assumptions: 
 *   - 1 nd_experiment_stock per nd_experiment
 *   - 1 nd_experiment_project per nd_experiment
 *   - 1+ nd_experiment_genotype per nd_experiment
 *   - 1 feature per genotype
 *
 * Columns:
 *   - stock_id: unique key of the stock the genotype was sampled from
 *   - stock_name: the human-readable name of the stock the genotype was sampled from
 *   - stock_experiment_relationship_type_id: role stock played in this experiment
 *   - stock_experiment_relationship_type_name: role stock played in this experiment
 *   - genotype_id: unique key of genotype resulting from experiment
 *   - genotype_uniquename: unique name of genotype resulting from experiment
 *   - genotype_description: genotype resulting from experiment
 *   - feature_genotype_relationship_type_id: how genotype is connected to feature
 *   - feature_genotype_relationship_type_name: how genotype is connected to feature
 *   - feature_id: the unique key of the feature the genotype is connected to
 *   - feature_name: the human-readable name of the feature the genotype is connected to
 *   - nd_experiment_id: the genotype experiment unique key
 *   - nd_geolocation_id: unique key of the geolocation where this experiment was performed
 *   - nd_geolocation_description: description of the geolocation where this experiment was performed
 *   - project_id: the unique key of the project this genotype experiment is associated with
 *   - project_name: the name of the project this genotype experiment is associated with
 *   - nd_genotype_experiment_id: unique key for this table
 */
function nd_genotypes_register_nd_genotype_experiment_mview () {

	$view_name = 'nd_genotype_experiment';
	$schema_definition = 'stock_id integer,
												stock_name varchar(255),
												stock_experiment_relationship_type_id integer,
												stock_experiment_relationship_type_name varchar(255),
												genotype_id integer,
												genotype_uniquename text,
												genotype_description text,
												feature_genotype_relationship_type_id integer,
												feature_genotype_relationship_type_name varchar(255),
												feature_id integer,
												feature_name varchar(255),
												nd_experiment_id integer,
												nd_geolocation_id integer,
												nd_geolocation_description varchar(255),
												project_id integer,
												project_name varchar(255),
												nd_genotype_experiment_id serial';
	$sql = "SELECT 
						s.stock_id, 
						s.name as stock_name, 
						ndes.type_id as stock_experiment_relationship_type_id,
						ndes_cvt.name as stock_experiment_relationship_type_name,
						g.genotype_id as genotype_id, 
						g.uniquename as genotype_uniquename, 
						g.description as genotype_description, 
						fg.cvterm_id as feature_genotype_relationship_type_id,
						fg_cvt.name as feature_genotype_relationship_type_name,
						f.feature_id as feature_id, 
						f.name as feature_name, 
						nde.nd_experiment_id, 
						nde.nd_geolocation_id, 
						ndg.description as nd_geolocation_description,
						p.project_id, 
						p.name as project_name
					FROM nd_experiment nde 
					LEFT JOIN nd_experiment_project ndep ON ndep.nd_experiment_id=nde.nd_experiment_id 
					LEFT JOIN nd_geolocation ndg ON ndg.nd_geolocation_id=nde.nd_geolocation_id
					LEFT JOIN project p ON p.project_id=ndep.project_id 
					LEFT JOIN nd_experiment_stock ndes ON ndes.nd_experiment_id=nde.nd_experiment_id 
					LEFT JOIN stock s ON s.stock_id=ndes.stock_id 
					LEFT JOIN cvterm ndes_cvt ON ndes_cvt.cvterm_id=ndes.type_id
					LEFT JOIN nd_experiment_genotype ndeg ON ndeg.nd_experiment_id=nde.nd_experiment_id 
					LEFT JOIN genotype g ON g.genotype_id=ndeg.genotype_id 
					LEFT JOIN feature_genotype fg ON fg.genotype_id=g.genotype_id 
					LEFT JOIN cvterm fg_cvt ON fg_cvt.cvterm_id=fg.cvterm_id
					LEFT JOIN feature f ON f.feature_id=fg.feature_id";

    $mview_id = tripal_mviews_get_mview_id($view_name);
    if (!$mview_id) {
      //create
      tripal_add_mview(
        // view name
        $view_name,
        // module creating materialized view
        'nd_genotypes',
        // view table name
        $view_name,
        // schema definition
        $schema_definition,
        // columns to index
        'nd_genotype_experiment_id',
        // SQL to populate materialized view
        $sql,
        // special index
        ''
      );
      $mview_id = tripal_mviews_get_mview_id($view_name);
    }
    
    //Populate
    tripal_update_mview($mview_id);
}