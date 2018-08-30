/**
 * Tests that the $out aggregation stage can read majority reads when the readConcern level is
 * "majority".
 *
 * @tags: [requires_replication]
 */
(function() {
    "use strict";

    load("jstests/libs/write_concern_util.js");  // For stopReplicationOnSecondaries().

    const rst = new ReplSetTest({name: "aggregationOutMajorityReads", nodes: 2});
    const nodes = rst.startSet();
    rst.initiate();

    const dbName = "aggregation_out_majority";
    const primaryDB = rst.getPrimary().getDB(dbName);
    const sourceColl = primaryDB.source;
    const outputColl = primaryDB.output;

    // Create the source collection and ensure that we have a majority-commited write.
    assert.commandWorked(
        sourceColl.insert({_id: 0, state: "old"}, {writeConcern: {w: "majority"}}));
    assert.commandWorked(outputColl.createIndex({state: 1}, {unique: true}));

    // "replaceCollection" mode is not supported with readConcern level "majority".
    assert.commandFailedWithCode(sourceColl.runCommand("aggregate", {
        cursor: {},
        pipeline: [{$out: {to: outputColl.getName(), mode: "replaceCollection"}}],
        readConcern: {level: "majority"}
    }),
                                 ErrorCodes.InvalidOptions);

    // An aggregation using readConcern level "majority" can read committed data.
    const kReadConcernMajority = {readConcern: {level: "majority"}};
    ["insertDocuments", "replaceDocuments"].forEach(mode => {
        assert.doesNotThrow(() => sourceColl.aggregate([{
                                                          $out: {
                                                              to: outputColl.getName(),
                                                              mode: mode,
                                                              uniqueKey: {state: 1},
                                                          }
                                                       }],
                                                       kReadConcernMajority));
        assert.eq([{_id: 0, state: "old"}],
                  outputColl.find({}, {}, kReadConcernMajority).toArray());
    });

    // Stop replication on all secondaries to prevent the majority commit point from advancing.
    stopReplicationOnSecondaries(rst);

    // Perform a write that will be majority committed (yet).
    assert.commandWorked(sourceColl.update({state: "old"}, {$set: {state: "new"}}));
}());
